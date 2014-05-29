#ifndef FORMWIDGET_H
#define FORMWIDGET_H

#include "widget.h"
#include <QRegExp>
#include <QString>

class FormWidget : public Widget
{
private:
    QRegExp _regexp;
    QString _name;
    int _nameWidth;
    QString _value;
    bool _readOnly;
public:
    FormWidget(QString name, int nameWidth, QString pattern=""); 
    bool isFocusable() const;
    int draw(int row, int margin);
    bool keyPressed(int key);
    void setValue(const QString& value);
    QString value() const;
    void setReadOnly(bool readOnly);
    bool isValid() const;
};

#endif // FORMWIDGET_H
