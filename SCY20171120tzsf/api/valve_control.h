#ifndef VALVE_CONTROL_H
#define VALVE_CONTROL_H

//#include <QObject>
//#include "api/myapp.h"
//#include "api/message.h"
//#include <QList>


class Valve_control
{

public:
//    static int   Valve_Set_Count;
//    static bool Set_Flag;
    static bool Valve_CurrentStatus;

    static void Catchment_Valve_Open_Set();
    static void Catchment_Valve_Close_Set();
    static void drain_Valve_Open_Set();
    static void drain_Valve_Close_Set();

};




#endif // VALVE_CONTROL_H
