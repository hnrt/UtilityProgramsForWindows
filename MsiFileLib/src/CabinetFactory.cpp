#include "pch.h"
#include "hnrt/CabinetFactory.h"
#include "Cabinet.h"


using namespace hnrt;


ICabinet* CabinetFactory::Create()
{
    return new Cabinet();
}
