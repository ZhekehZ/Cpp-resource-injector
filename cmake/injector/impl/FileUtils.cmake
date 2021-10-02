MACRO(__RES_INJ_READ_FILE PATH VAR)
    FILE(READ ${${PATH}} ${VAR})
    STRING(REPLACE ";" "\\\;" ${VAR} "${${VAR}}")
    STRING(REPLACE "\n" "\\n" ${VAR} "${${VAR}}")
ENDMACRO()

MACRO(__RES_INJ_ASSERT_PATH PATH)
    IF (NOT EXISTS ${${PATH}})
        MESSAGE(FATAL_ERROR "FILE '${${PATH}}' IS NOT EXISTS")
    ENDIF()
ENDMACRO()