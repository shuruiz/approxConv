add_llvm_library( LLVMCONV MODULE
  convpass.cpp
  threshpass.cpp

  PLUGIN_TOOL
  opt
  )
