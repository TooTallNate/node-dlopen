
var Library = require('../');
var assert = require('assert');

describe('Library', function () {

  it('should return a `Library` instance', function () {
    // no arguments (or null) means to dlopen the currently running process
    var lib = new Library();
    assert(lib instanceof Library);
  });

});
