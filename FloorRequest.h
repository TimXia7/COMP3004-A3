#ifndef FLOORREQUEST_H
#define FLOORREQUEST_H

#include <QString>

/*
    FloorRequest Class

    In floor request representation
*/

class FloorRequest{

    public:
        FloorRequest(int targetFloor, QString direction);
        //using default destructor

        int getFloor() const ;
        QString getDirection() const ;

    private:
        const int targetFloor;
        const QString direction;
};

#endif
