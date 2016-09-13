#ifndef EXCEPTION_H
#define EXCEPTION_H

class Exception
{
public:
  const QString msg;
  Exception(const QString arg): msg(arg){}
};

#endif // EXCEPTION_H
