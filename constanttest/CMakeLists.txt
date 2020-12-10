add_llvm_library( LLVMCTP MODULE
  ctpass.cpp
  iiepass.cpp

  PLUGIN_TOOL
  opt
  )
