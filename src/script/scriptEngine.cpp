#include "scriptEngine.h"

#include <QtScript/QScriptValue>

ScriptEngine::ScriptEngine(QObject * parent)
    : QScriptEngine(parent)
{
    connect(this, &ScriptEngine::evaluateSignal, this, &ScriptEngine::evaluate);
}
ScriptEngine::~ScriptEngine()
{

}
void ScriptEngine::evaluate(const QString & str)
{
    QScriptEngine::evaluate(str);
}
/// A thread-safe evaluate()
void ScriptEngine::safeEvaluate(const QString & str)
{
    emit evaluateSignal(str);
}
