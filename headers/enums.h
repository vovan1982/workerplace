#ifndef ENUMS_H
#define ENUMS_H

#include <QtCore>

class Enums {
public:
    enum Modes {
        WorkPlace,
        Device,
        Standart
    };
    Q_ENUM(Modes)
    Q_GADGET
    Enums() = delete; // std=c++11, обеспечивает запрет на создание любого экземпляра Enums
};

#endif // ENUMS_H
