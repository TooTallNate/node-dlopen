
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
 * Re-export the *raw* dl functions.
 */

exports.dlopen = bindings.dlopen;
exports.dlclose = bindings.dlclose;
exports.dlsym = bindings.dlsym;
exports.dlerror = bindings.dlerror;

/**
 * Map of `process.platform` values to their corresponding
 * "dynamic library" file name extension.
 */

exports.ext = {
  linux:   '.so',
  linux2:  '.so',
  sunos:   '.so',
  solaris: '.so',
  freebsd: '.so',
  openbsd: '.so',
  darwin:  '.dylib',
  mac:     '.dylib',
  win32:   '.dll'
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
  if (!name) throw new TypeError('A string library name is required');

  // append the `ext` if necessary
  var ext = exports.ext[process.platform];
  if (name.substring(name.length - ext.length) !== ext) {
    name += ext;
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
    var message = bindings.dlerror(this.uv_lib_t);
    throw new Error(message);
  }
}

/**
 * Calls `uv_dlclose()` on this Library instance.
 *
 * @api public
 */

Library.prototype.close = function () {
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
  var sym = new Buffer(bindings.sizeof_void_ptr);
  var r = bindings.dlsym(this.uv_lib_t, name, sym);

  if (0 !== r) {
    // error
    var message = bindings.dlerror(this.uv_lib_t);
    sym = null;
    throw new Error(message);
  }

  // add the symbol "name"
  sym.name = name;

  return sym;
};
