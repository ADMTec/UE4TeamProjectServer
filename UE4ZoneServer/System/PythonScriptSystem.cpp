#include "PythonScriptSystem.hpp"


void PythonScript::System::Initialize()
{
    Py_Initialize();
}

void PythonScript::System::Exit()
{
    Py_Exit(0);
}