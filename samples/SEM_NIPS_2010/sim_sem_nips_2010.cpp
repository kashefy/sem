/** @file Run SEM simulation (NIPS 2010)
 */
#include <iostream>

#include "elm/core/core.h"
#include "simulationsem.h"

using namespace std;

int main() {

    cout<<elm::GetVersion()<<endl;

    SimulationSEM s;

    cout<<"Learn()"<<endl;

    s.Learn();

    cout<<"Test()"<<endl;

    //s.Test();

    cout<<"Eval()"<<endl;

    s.Eval();

    return 0;
}
