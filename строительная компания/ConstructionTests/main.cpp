#include <cassert>
#include <iostream>
#include "CoreGeo.h"
#include "Exceptions.h"
#include "Materials.h"

int main() {
    Material brick;
    brick.name = "Brick";
    brick.unit = "pcs";
    brick.price = 2.0;
    brick.weightPerUnit = 3.0;

    MaterialStock stock;
    stock.material = brick;
    stock.totalQty = 50.0;
    stock.minQty = 10.0;

    stock.writeOff(20.0);
    assert(stock.freeQty() == 30.0);

    bool caught = false;
    try {
        stock.writeOff(100.0);
    }
    catch (const InsufficientMaterialException&) {
        caught = true;
    }
    assert(caught);

    std::cout << "All checks passed!\n";
    return 0;
}