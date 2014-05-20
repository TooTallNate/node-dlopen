{
  'targets': [
    {
      'target_name': 'binding',
      'sources': [ 'src/binding.cc' ],
      'include_dirs': [
        '<!(node -e "require(\'nan\')")',
        '<(node_root_dir)/deps/uv/include'
      ],
    }
  ]
}
