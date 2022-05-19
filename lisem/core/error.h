#pragma once

#include <QString>
#include <QList>
#include <QMutex>
#include "iostream"
#include "defines.h"
#include <exception>

#define LISEM_MESSAGE_ERROR 0
#define LISEM_MESSAGE_WARNING 1
#define LISEM_MESSAGE_STATUS 2
#define LISEM_MESSAGE_DEBUG 3
#define LISEM_MESSAGE_SCRIPT 4

#define SAFE_DELETE(x) if(x != NULL){delete x;x = NULL;}
#define SAFE_DELETENS(x) if(x != NULL){delete x;;}

#define LISEM_CODE_RETURN_NORMAL 0x000000
#define LISEM_CODE_RETURN_RESTART 0x000001

extern QMutex *MessageMutex_ALL;
extern QList<QString> *Messages_ALL;
extern QList<int> *Levels_ALL;
extern bool MessageUseCommandLine;
extern bool MessageUseCommandLineS;
extern QMutex *MessageMutex;
extern QList<QString> *Messages;
extern QList<int> *Levels;

typedef struct LeveledMessage
{
    QString Message;
    int Level;
} LeveledMessage;

LISEM_API void InitMessages(bool use_commandline);
LISEM_API void AddMessage(QString message, int level);
LISEM_API QList<LeveledMessage> GetMessages();
LISEM_API QList<LeveledMessage> GetMessages_ALL();

extern bool LISEM_use_cout;
LISEM_API void SetPrintUseStdOut(bool x);
LISEM_API void PrintQStringStdOut(const QString &s);

#define LISEM_ERROR(x) AddMessage(x,LISEM_MESSAGE_ERROR);

#define LISEM_WARNING(x) AddMessage(x,LISEM_MESSAGE_WARNING);

#define LISEM_STATUS(x) AddMessage(x,LISEM_MESSAGE_STATUS);

#define LISEM_DEBUG(x) AddMessage(x,LISEM_MESSAGE_DEBUG);

#define LISEM_SCRIPTOUTPUT(x) AddMessage(x,LISEM_MESSAGE_SCRIPT);


extern QMutex *MessageMutexS; // S means Script
extern QList<QString> *MessagesS;
extern QList<int> *LevelsS;


extern QMutex *MessageMutexSTD; //STD means Script Tool Dialog
extern QList<QString> *MessagesSTD;
extern QList<int> *LevelsSTD;

LISEM_API void InitMessagesS(bool use_commandline);
LISEM_API void AddMessageS(QString message, int level);
LISEM_API QList<LeveledMessage> GetMessagesS();
LISEM_API QList<LeveledMessage> GetMessagesSTD();
LISEM_API void PrintLISEMDebug();

#define LISEMS_ERROR(x) AddMessageS(x,LISEM_MESSAGE_ERROR);

#define LISEMS_WARNING(x) AddMessageS(x,LISEM_MESSAGE_WARNING);

#define LISEMS_STATUS(x) AddMessageS(x,LISEM_MESSAGE_STATUS);

#define LISEMS_DEBUG(x) AddMessageS(x,LISEM_MESSAGE_DEBUG);

#define LISEMS_SCRIPTOUTPUT(x) AddMessageS(x,LISEM_MESSAGE_SCRIPT);

#define LISEM_PRINT_DEBUG(x) PrintLISEMDebug();

LISEM_API QString getStringFromUnsignedChar( const unsigned char *str );
LISEM_API QString GetLastErrorMessage();

class LISEM_API Exception: public std::exception
{
public:
    /** Constructor (C strings).
     *  @param message C-style string error message.
     *                 The string contents are copied upon construction.
     *                 Hence, responsibility for deleting the char* lies
     *                 with the caller.
     */
    explicit Exception(const char* message)
        : msg_(message) {}

    /** Constructor (C++ STL strings).
     *  @param message The error message.
     */
    explicit Exception(const std::string& message)
        : msg_(message.c_str()) {}

    explicit Exception(const QString& message)
        : msg_(message) {}

    /** Destructor.
     * Virtual to allow for subclassing.
     */
    virtual ~Exception() noexcept {}

    /** Returns a pointer to the (constant) error description.
     *  @return A pointer to a const char*. The underlying memory
     *          is in posession of the Exception object. Callers must
     *          not attempt to free the memory.
     */
    virtual const char* what() const noexcept {
       return msg_.toStdString().c_str();
    }

protected:
    /** Error message.
     */
    QString msg_;
};
