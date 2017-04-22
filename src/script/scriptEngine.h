#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H

#include <QtScript/QScriptEngine>

class ScriptEngine : public QScriptEngine
{
    Q_OBJECT
    Q_SIGNAL void evaluateSignal(const QString &);
public:
    explicit ScriptEngine(QObject * parent = 0);
    ~ScriptEngine();

    Q_SLOT void evaluate(const QString & str);
    /// A thread-safe evaluate()
    Q_SLOT void safeEvaluate(const QString & str);
};


#endif // SCRIPTENGINE_H
