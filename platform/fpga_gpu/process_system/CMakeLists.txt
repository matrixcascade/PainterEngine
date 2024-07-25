#################################################################
#PainterEngine Directory
set(PAINTERENGINE_DIR D:/working/PainterEngine)
#################################################################

include_directories(${PAINTERENGINE_DIR})
include_directories(${PAINTERENGINE_DIR}/runtime)

aux_source_directory(${CMAKE_SOURCE_DIR} _sources)
aux_source_directory(${PAINTERENGINE_DIR}/core _sources)
aux_source_directory(${PAINTERENGINE_DIR}/kernel _sources)

