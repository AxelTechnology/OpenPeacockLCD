#include "module.h"

Module::Module(QObject *parent) : QObject(parent)
{

}

Module::~Module()
{
    // destructor
}

void Module::initModule()
{
}

void Module::moduleParseData(QString* parserName)
{
    // insert here parsing data code
}

void Module::modulePaintData(QString* painterName)
{
    // insert here paint code
}
