add_llvm_library( LLVMCONV MODULE
  convpass.cpp
  threshpass.cpp
  convpass5.cpp	

  PLUGIN_TOOL
  opt
  )
