

#include "qsslsocket.h"
#include "lisem.h"



int main(int argc, char *argv[])
{
    std::cout << "qssl "  << QSslSocket::sslLibraryBuildVersionString().toStdString() << std::endl;

    SPHazard sphazard;

    int errorcode = sphazard.execute(argc,argv);


    return 0;
}

