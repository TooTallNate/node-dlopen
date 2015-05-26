
/**
 * Module dependencies.
 */

var debug = require('debug')('dlopen');
var bindings = require('bindings')('binding');

/**
 * Module exports.
 */

exports = module.exports = Library;

/**
 * Re-export the *raw* dl bindings.
 */

Object.keys(bindings).forEach(function (key) {
  exports[key] = bindings[key];
});

/**
 * Map of `process.platform` values to their corresponding
 * "dynamic library" file name extension.
 */

exports.ext = {
  linux:   ['.so'],
  linux2:  ['.so'],
  sunos:   ['.so'],
  solaris: ['.so'],
  freebsd: ['.so'],
  openbsd: ['.so'],
  darwin:  ['.dylib', '.bundle', '.framework'],
  mac:     ['.dylib', '.bundle', '.framework'],
  win32:   ['.dll']
};

/**
 * The `Library` class is an object-oriented wrapper around the
 * dlopen(), dlclose(), dlsym() and dlerror() functions.
 *
 * @param {String} name - Library name or full filepath
 * @api private
 */

function Library (name) {
  if (!(this instanceof Library)) return new Library(name);

  if (name) {
    // append the `ext` if necessary
    var exts = exports.ext[process.platform];
    var mustAppendExt = true;
    exts.forEach(function(ext) {
      if (name.substring(name.length - ext.length) === ext) {
        mustAppendExt = false;
      }
    });

    if (mustAppendExt) {
      debug('appending dynamic lib suffix (%s)', exts[0], name);
      name += exts[0];
    }
  } else {
    // if no name was passed in then pass `null` to open the current process
    name = null;
  }

  debug('library name', name);
  this.name = name;

  // create the `uv_lib_t` data space
  this.uv_lib_t = new Buffer(bindings.sizeof_uv_lib_t);

  // do the `dlopen()` dance
  var r = bindings.dlopen(name, this.uv_lib_t);
  debug('dlopen() result', r);
  if (0 !== r) {
    // error
    throw new Error(bindings.dlerror ?
        bindings.dlerror(this.uv_lib_t) :
        'Unable to load shared library');
  }
}

/**
 * Calls `uv_dlclose()` on this Library instance.
 *
 * @api public
 */

Library.prototype.close = function () {
  debug('close()');
  if (this.uv_lib_t) {
    bindings.dlclose(this.uv_lib_t);
    this._uv_lib_t = null;
  }
};

/**
 * Calls `uv_dlsym()` on this Library instance.
 *
 * A Node.js `Buffer` instance is returned which points to the
 * memory location of the requested "symbol".
 *
 * @param {String} name - Symbol name to attempt to retrieve
 * @return {Buffer} a Buffer instance pointing to the memory address of the symbol
 * @api public
 */

Library.prototype.get = function (name) {
  debug('get()', name);
  var sym = new Buffer(bindings.sizeof_void_ptr);
  var r = bindings.dlsym(this.uv_lib_t, name, sym);
  debug('dlsym() result', r);
  if (0 !== r) {
    // error
    sym = null;
    throw new Error(bindings.dlerror ?
        bindings.dlerror(this.uv_lib_t) :
        'Unable to load symbol');
  }

  // add some debugging info
  sym.name = name;
  sym.lib = this;

  return sym;
};
