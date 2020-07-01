#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
//for creating directories
#include <sys/stat.h>
#include <sys/types.h>
//for opening directories
#include <dirent.h>
#include <errno.h>

#define MAX_SOURCE_FILE_NAME_LENGTH 20
#define MAX_TOKEN_LINE_LENGTH 400
#define MAX_TOKEN_LENGTH 50
#define MAX_DIR_LENGTH 100

char *file;
unsigned long long currentIndex = 0;
FILE *sourceFileWrite;

// BNF
/*
ProgramBegin() -> ifStatement() | commandStatement() | NULL
ifStatement() -> "if" + pathStatement() + commandStatement() | blockStatement()
pathStatement() -> "<" + "abc...z,*,_,/" + ">"
commandStatement() -> "make" | "go" + pathStatement() + ";" + ProgramBegin()
blockStatement() -> "{" + ProgramBegin() + "}" + ProgramBegin()
*/

//Prototyping functions//
void isPath();
void isCommand();
void isOther();
//parser//
void ProgramBegin();
void ifStatement();
void ifNotStatement();
void blockStatement(char data[], char ifResult);
void commandStatement();
char * pathStatement();
void EndOfLineStatement();
void LeftBracketStatement();
void RightBracketStatement();
//End Of Prototyping//

char CurrentDirectory[MAX_DIR_LENGTH] = "";
char TempCurrentDirectory[MAX_DIR_LENGTH] = "";
int CurrentDirectoryIndex = 0;
int globalCounter = 0;

const char *fullTokenList[MAX_TOKEN_LINE_LENGTH][MAX_TOKEN_LENGTH];
const char *fullTokenTypeList[MAX_TOKEN_LINE_LENGTH][MAX_TOKEN_LENGTH];

int main()
{
    FILE *sourceFile;
    printf("ENTER THE SOURCE FILE NAME: ");
    char sourceFileName[MAX_SOURCE_FILE_NAME_LENGTH] = "";
    scanf("%s", sourceFileName);
    char tempFileName[MAX_SOURCE_FILE_NAME_LENGTH+4] = "";
    int fileNameIndex = 0;
    for(fileNameIndex = 0; fileNameIndex< strlen(sourceFileName); fileNameIndex++)
    {
        tempFileName[fileNameIndex] = sourceFileName[fileNameIndex];
    }
    tempFileName[fileNameIndex] = '.';
    tempFileName[++fileNameIndex] = 'p';
    tempFileName[++fileNameIndex] = 'm';
    tempFileName[++fileNameIndex] = 'k';
    printf("OPENING %s FILE...\n", tempFileName);

    sourceFile = fopen(tempFileName, "r");
    sourceFileWrite = fopen("code.lex", "w+");

    unsigned long long fileLengthCounter = 0;
    if(sourceFile == NULL)
    {
        printf("ERROR: %s FILE IS NOT FOUND! \n", tempFileName);
        printf("ABORTING...\n");
        exit(0);
    }
    else
    {
        while(!feof(sourceFile))
        {
            fileLengthCounter++;
            fgetc(sourceFile);
        }
    }

    file = (char *) calloc(fileLengthCounter, sizeof(char)); //file source dosyasini hafizada tutar
    if(file)
    {
        printf("SUCCESS: %s FILE IS READY! \n\n", tempFileName);
    }
    rewind(sourceFile);
    fileLengthCounter=0;
    while(!feof(sourceFile))
    {
        file[fileLengthCounter] = fgetc(sourceFile);  // dinamik olusturdugumuz diziyi dolduruyoruz
        fileLengthCounter++;
    }
    fclose( sourceFile );

    // MAIN LOGIC //
    while(currentIndex < fileLengthCounter)
    {
        isPath(); //path data type tespit eder
        isCommand(); //command tespit eder
        isOther();
        currentIndex++; //en altta birak
    }
    fclose( sourceFileWrite );

    sourceFileWrite = fopen("code.lex", "r");
    char *rawToken = (char *) calloc(MAX_TOKEN_LENGTH, sizeof(char));
    char *token = (char *) calloc(MAX_TOKEN_LENGTH, sizeof(char));
    char *tokenVariable = (char *) calloc(MAX_TOKEN_LENGTH, sizeof(char));

    int fullTokenListIndex = 0;
    while( fgets (rawToken, MAX_TOKEN_LENGTH, sourceFileWrite)!=NULL )
    {
        if(rawToken[0] == '.')
        {
            int tokenIndex = 1;
            while(rawToken[tokenIndex] != '(')
            {
                token[tokenIndex-1] = rawToken[tokenIndex];
                tokenIndex++;
            }

            tokenIndex++;
            int tokenVarIndex = 1;
            while(rawToken[tokenIndex] != ')')
            {
                tokenVariable[tokenVarIndex-1] = rawToken[tokenIndex];
                tokenIndex++;
                tokenVarIndex++;
            }

            int i = 0;
            while(tokenVariable[i]) //null gelesiye kadar atama yap
            {
                fullTokenList[fullTokenListIndex][i] = tokenVariable[i];
                i++;
            }

            i = 0;
            while(token[i]) //null gelesiye kadar atama yap
            {
                fullTokenTypeList[fullTokenListIndex][i] = token[i];
                i++;
            }
            fullTokenListIndex++;
        }
        else
        {
            //printf("single token geldi: %s\n",rawToken);
            int i = 0;
            while(rawToken[i+1]) //null gelesiye kadar atama yap, +1 sondaki newline almamak icin
            {
                fullTokenList[fullTokenListIndex][i] = rawToken[i];
                fullTokenTypeList[fullTokenListIndex][i] = rawToken[i];
                i++;
            }
            fullTokenListIndex++;
        }
        memset(rawToken, 0, MAX_TOKEN_LENGTH*sizeof(char));
        memset(token, 0, MAX_TOKEN_LENGTH*sizeof(char));
        memset(tokenVariable, 0, MAX_TOKEN_LENGTH*sizeof(char));
    }

    printf("SOURCE CODE TOKENS: \n");
    int k = 0;
    int j = 0;
    int lexLengthCounter = 0;
    for (k = 0; k < MAX_TOKEN_LINE_LENGTH; ++k)
    {
        if(!fullTokenList[k][0])
        {
            break;
        }
        printf("%d) ", k);
        for(j = 0; j < MAX_TOKEN_LENGTH; ++j)
        {
            printf("%c",fullTokenList[k][j]);
        }
        lexLengthCounter++;
        printf("\n");
    }
    printf("----------\n\n\n");

    printf("SOURCE CODE TOKEN TYPES: \n");
    k = 0;
    j = 0;
    for (k = 0; k < MAX_TOKEN_LINE_LENGTH; ++k)
    {
        if(!fullTokenTypeList[k][0])
        {
            break;
        }
        printf("%d) ", k);
        for(j = 0; j < MAX_TOKEN_LENGTH; ++j)
        {
            printf("%c",fullTokenTypeList[k][j]);
        }
        printf("\n");
    }
    printf("----------\n\n\n");

    /*PARSER*/
    printf("\n\nPARSER\n");
    ProgramBegin();

    if(globalCounter != lexLengthCounter)  //eger dosyanin sonu gelmeden bittiyse error ver
    {
        printf("\nERROR: PARSING FAILED DUE TO INVALID OPERATIONS!\n");
    }

    return 0;
}


void ProgramBegin(char data[])
{
    int tempC = 0;
    char tempWord[MAX_TOKEN_LENGTH] = "";
    char tempWordType[MAX_TOKEN_LENGTH] = "";
    while(fullTokenList[globalCounter][tempC])
    {
        tempWord[tempC] = fullTokenList[globalCounter][tempC];
        tempC++;
    }
    tempC=0;
    while(fullTokenTypeList[globalCounter][tempC])
    {
        tempWordType[tempC] = fullTokenTypeList[globalCounter][tempC];
        tempC++;
    }

    if(tempWord[0] == '\0')
    {
        printf("\n\nPARSER STATUS: DONE!\n");
    }
    else
    {
        printf("\n\n      INCOMING WORD TYPE: %s\n", tempWord);
    }

    if(strcmp(tempWord, "if") == 0)
    {
        globalCounter++;
        ifStatement();
    }
    else if(strcmp(tempWord, "ifnot") == 0)
    {
        globalCounter++;
        ifNotStatement();
    }
    else if(strcmp(tempWordType, "Command") == 0)
    {
        commandStatement(data);
    }
}


void ifStatement()
{
    char *pathTemp;
    pathTemp = pathStatement("x");
    int tempC = 0;
    char tempWord[MAX_TOKEN_LENGTH] = "";
    char tempWordType[MAX_TOKEN_LENGTH] = "";
    while(fullTokenList[globalCounter][tempC])
    {
        tempWord[tempC] = fullTokenList[globalCounter][tempC];
        tempC++;
    }
    tempC=0;
    while(fullTokenTypeList[globalCounter][tempC])
    {
        tempWordType[tempC] = fullTokenTypeList[globalCounter][tempC];
        tempC++;
    }

    if(strcmp(tempWordType, "LeftCurlyBracket") == 0 || strcmp(tempWordType, "Command") == 0)
    {
        printf("CHECK: if %s EXISTS,\n", pathTemp);
        int tmpI = 0;
        while(pathTemp[tmpI])
        {
            pathTemp[tmpI] = pathTemp[tmpI+1];
            tmpI++;
        }
        pathTemp[tmpI-1] = '\0';

        char storePath[MAX_TOKEN_LENGTH] = "";
        int rootSayac = 0;
        int storePathIndex = 0;
        int tempPathIndexNormal = 0;
        int goPathFailed = 0; //dosya yoksa 1 olacak
        memset(TempCurrentDirectory, 0, sizeof(TempCurrentDirectory));
        strcat(TempCurrentDirectory, CurrentDirectory); //TempCurrentDirectory <= CurrentDirectory
        while(pathTemp[tempPathIndexNormal])
        {
            if(pathTemp[tempPathIndexNormal] != '/' && pathTemp[tempPathIndexNormal+1] != '\0')
            {
                storePath[storePathIndex++] = pathTemp[tempPathIndexNormal];
            }
            else
            {
                if(storePath[0] == '*')
                {

                    if(CurrentDirectory[0] == '\0')
                    {
                        printf("\nWARNING: YOU ARE TRYING TO GO UP BUT YOU ARE ALREADY ON A ROOT DIRECTORY!\n\n");
                        goPathFailed = 1;
                        break;
                    }
                    else
                    {

                        //remove one directory from tempCurrentDirectory
                        //temp yap cunku if false verirse currentDirectory degismesin(dokumanda yaziyor!)

                        int silmeCounter = strlen(TempCurrentDirectory) -1;
                        int slashCounter = 0;
                        while(slashCounter != 2)  //ters dolas
                        {
                            if(TempCurrentDirectory[silmeCounter] == '/' || silmeCounter == -1)
                            {
                                slashCounter++;
                            }
                            else
                            {
                                TempCurrentDirectory[silmeCounter] = '\0';
                            }
                            silmeCounter--;
                        }
                    }
                }

                if(storePath[0] != '*')
                {
                    strcat(TempCurrentDirectory, storePath);
                    strcat(TempCurrentDirectory, "/");
                }

                printf("CHECKING: %s\n", TempCurrentDirectory);


                if(TempCurrentDirectory[0] == '\0')
                {
                    rootSayac++;
                }

                if(rootSayac >1)
                {
                    printf("\nWARNING: YOU ARE TRYING TO GO UP BUT YOU ARE ALREADY ON A ROOT DIRECTORY!\n\n");
                    goPathFailed = 1;
                    break;
                }


                DIR* dir = opendir(TempCurrentDirectory);
                if (dir)
                {
                    // Directory exists.
                    //closedir(dir);
                }
                else
                {
                    if(TempCurrentDirectory[0] == '\0')  //TempCurrentDirectory bos ise root dayiz ve hata verme
                    {
                        // Directory exists.
                    }
                    else
                    {
                        goPathFailed = 1;
                        printf("FILE WARNING: DIRECTORY \"%s\" IS NOT AVAILABLE!\n",TempCurrentDirectory);
                    }
                }

                memset(storePath,0,sizeof storePath);
                storePathIndex=0;
            }
            tempPathIndexNormal++;
        }


        if (goPathFailed == 0)
        {
            printf("IF STATUS: TRUE\n");
            if(strcmp(tempWordType, "Command") == 0)
            {
                //globalCounter++;
                blockStatement(pathTemp, '=');  //"equals" means if statement is true but it has no block so don't expect curly brackets
            }
            else
            {
                blockStatement(pathTemp, '+');  //"+" means if statement is true so expect curly brackets
            }
        }
        else
        {
            printf("IF STATUS: FALSE\n");
            //pass if state because it is wrong!
            if(strcmp(tempWordType, "Command") == 0)
            {
                globalCounter+=3; //command, path, endofline gec
                blockStatement(pathTemp, '-');
            }
            else
            {
                // GLOBAL COUNTER'I RIGHT CURLY BRACKET GORESIYE KADAR ILERLET
                char tempTokenData[MAX_TOKEN_LENGTH] = "";
                int tempSolBracketSay = 0;
                int tempSagBracketSay = 0;
                //yapilan islem: if sonucu false ise if blogunu ve icidekileri atla
                while(1) //sag braketlerin sayisi sol braketlerden fazla olasiya kadar calis
                {
                    memset(tempTokenData, 0, sizeof tempTokenData);
                    int tempTokenIndex = 0;
                    while(fullTokenTypeList[globalCounter][tempTokenIndex])
                    {
                        tempTokenData[tempTokenIndex] = fullTokenTypeList[globalCounter][tempTokenIndex];
                        tempTokenIndex++;
                    }
                    if(strcmp(tempTokenData, "LeftCurlyBracket") == 0)
                    {
                        tempSolBracketSay++;
                    }
                    else if(strcmp(tempTokenData, "RightCurlyBracket") == 0)
                    {
                        tempSagBracketSay++;
                    }
                    globalCounter++;
                    if(tempSolBracketSay == tempSagBracketSay)
                    {
                        break;
                    }
                }
                globalCounter--; //RightCurlyBracket den bir oncekini isaret et cunku RightCurlyBracket fonksiyonunda kendisi bir arttýrýyor!

                blockStatement(pathTemp, '-');
            }
        }
    }
    else if(strcmp(tempWordType, "Command") == 0)
    {
        commandStatement(pathTemp);
    }
    else
    {
        printf("\nPARSE ERROR(1): \"LeftCurlyBracket/Command\" IS EXPECTED BUT GIVEN \"%s\"\n\n", tempWord);
    }
}


void ifNotStatement()
{
    char *pathTemp;
    pathTemp = pathStatement("x");
    int tempC = 0;
    char tempWord[MAX_TOKEN_LENGTH] = "";
    char tempWordType[MAX_TOKEN_LENGTH] = "";
    while(fullTokenList[globalCounter][tempC])
    {
        tempWord[tempC] = fullTokenList[globalCounter][tempC];
        tempC++;
    }
    tempC=0;
    while(fullTokenTypeList[globalCounter][tempC])
    {
        tempWordType[tempC] = fullTokenTypeList[globalCounter][tempC];
        tempC++;
    }

    if(strcmp(tempWordType, "LeftCurlyBracket") == 0 || strcmp(tempWordType, "Command") == 0)
    {
        printf("CHECK: ifnot %s EXISTS,\n", pathTemp);
        int tmpI = 0;
        while(pathTemp[tmpI])
        {
            pathTemp[tmpI] = pathTemp[tmpI+1];
            tmpI++;
        }
        pathTemp[tmpI-1] = '\0';

        char storePath[MAX_TOKEN_LENGTH] = "";
        int rootSayac = 0;
        int storePathIndex = 0;
        int tempPathIndexNormal = 0;
        int goPathFailed = 0; //dosya yoksa 1 olacak
        memset(TempCurrentDirectory, 0, sizeof(TempCurrentDirectory));
        strcat(TempCurrentDirectory, CurrentDirectory); //TempCurrentDirectory <= CurrentDirectory
        while(pathTemp[tempPathIndexNormal])
        {
            if(pathTemp[tempPathIndexNormal] != '/' && pathTemp[tempPathIndexNormal+1] != '\0')
            {
                storePath[storePathIndex++] = pathTemp[tempPathIndexNormal];
            }
            else
            {
                if(storePath[0] == '*')
                {

                    if(CurrentDirectory[0] == '\0')
                    {
                        printf("\nWARNING: YOU ARE TRYING TO GO UP BUT YOU ARE ALREADY ON A ROOT DIRECTORY!\n\n");
                        goPathFailed = 1;
                        break;
                    }
                    else
                    {

                        //remove one directory from tempCurrentDirectory
                        //temp yap cunku if false verirse currentDirectory degismesin(dokumanda yaziyor!)

                        int silmeCounter = strlen(TempCurrentDirectory) -1;
                        int slashCounter = 0;
                        while(slashCounter != 2)  //ters dolas
                        {
                            if(TempCurrentDirectory[silmeCounter] == '/' || silmeCounter == -1)
                            {
                                slashCounter++;
                            }
                            else
                            {
                                TempCurrentDirectory[silmeCounter] = '\0';
                            }
                            silmeCounter--;
                        }
                    }
                }

                if(storePath[0] != '*')
                {
                    strcat(TempCurrentDirectory, storePath);
                    strcat(TempCurrentDirectory, "/");
                }

                printf("CHECKING: %s\n", TempCurrentDirectory);


                if(TempCurrentDirectory[0] == '\0')
                {
                    rootSayac++;
                }

                if(rootSayac >1)
                {
                    printf("\nWARNING: YOU ARE TRYING TO GO UP BUT YOU ARE ALREADY ON A ROOT DIRECTORY!\n\n");
                    goPathFailed = 1;
                    break;
                }


                DIR* dir = opendir(TempCurrentDirectory);
                if (dir)
                {
                    // Directory exists.
                    //closedir(dir);
                }
                else
                {
                    if(TempCurrentDirectory[0] == '\0')  //TempCurrentDirectory bos ise root dayiz ve hata verme
                    {
                        // Directory exists.
                    }
                    else
                    {
                        goPathFailed = 1;
                        //printf("FILE WARNING: DIRECTORY \"%s\" IS NOT AVAILABLE!\n",TempCurrentDirectory);
                    }
                }

                memset(storePath,0,sizeof storePath);
                storePathIndex=0;
            }
            tempPathIndexNormal++;
        }


        if (goPathFailed == 1)
        {
            printf("IFNOT STATUS: TRUE\n");
            if(strcmp(tempWordType, "Command") == 0)
            {
                //globalCounter++;
                blockStatement(pathTemp, '=');  //"equals" means if statement is true but it has no block so don't expect curly brackets
            }
            else
            {
                blockStatement(pathTemp, '+');  //"+" means if statement is true so expect curly brackets
            }
        }
        else
        {
            printf("IFNOT STATUS: FALSE\n");
            //pass if state because it is wrong!
            if(strcmp(tempWordType, "Command") == 0)
            {
                globalCounter+=3; //command, path, endofline gec
                blockStatement(pathTemp, '-');
            }
            else
            {
                // GLOBAL COUNTER'I RIGHT CURLY BRACKET GORESIYE KADAR ILERLET
                char tempTokenData[MAX_TOKEN_LENGTH] = "";
                int tempSolBracketSay = 0;
                int tempSagBracketSay = 0;
                //yapilan islem: if sonucu false ise if blogunu ve icidekileri atla
                while(1) //sag braketlerin sayisi sol braketlerden fazla olasiya kadar calis
                {
                    memset(tempTokenData, 0, sizeof tempTokenData);
                    int tempTokenIndex = 0;
                    while(fullTokenTypeList[globalCounter][tempTokenIndex])
                    {
                        tempTokenData[tempTokenIndex] = fullTokenTypeList[globalCounter][tempTokenIndex];
                        tempTokenIndex++;
                    }
                    if(strcmp(tempTokenData, "LeftCurlyBracket") == 0)
                    {
                        tempSolBracketSay++;
                    }
                    else if(strcmp(tempTokenData, "RightCurlyBracket") == 0)
                    {
                        tempSagBracketSay++;
                    }
                    globalCounter++;
                    if(tempSolBracketSay == tempSagBracketSay)
                    {
                        break;
                    }
                }
                globalCounter--; //RightCurlyBracket den bir oncekini isaret et cunku RightCurlyBracket fonksiyonunda kendisi bir arttýrýyor!

                blockStatement(pathTemp, '-');
            }
        }
    }
    else if(strcmp(tempWordType, "Command") == 0)
    {
        commandStatement(pathTemp);
    }
    else
    {
        printf("\nPARSE ERROR(1): \"LeftCurlyBracket/Command\" IS EXPECTED BUT GIVEN \"%s\"\n\n", tempWord);
    }
}


void blockStatement(char data[], char ifResult) //ifResult true if if statement is true, else false
{
    if(ifResult == '+')
    {
        LeftBracketStatement();
    }

    int tempC = 0;
    char tempWord[MAX_TOKEN_LENGTH] = "";
    char tempWordType[MAX_TOKEN_LENGTH] = "";
    while(fullTokenList[globalCounter][tempC])
    {
        tempWord[tempC] = fullTokenList[globalCounter][tempC];
        tempC++;
    }
    tempC=0;
    while(fullTokenTypeList[globalCounter][tempC])
    {
        tempWordType[tempC] = fullTokenTypeList[globalCounter][tempC];
        tempC++;
    }

    if(strcmp(tempWordType, "RightCurlyBracket") == 0)
    {
        globalCounter++;
    }
    else if(strcmp(tempWordType, "Command") == 0)
    {
        commandStatement(data);
    }
    else if(strcmp(tempWord, "if") == 0) //unutulmus, sonradan ekledim
    {
        globalCounter++;
        ifStatement();
    }
    else
    {
        printf("\nPARSE ERROR(2): \"LeftCurlyBracket/RightCurlyBracket/Command\" IS EXPECTED BUT GIVEN \"%s\"\n\n", tempWord);
    }

    if(ifResult == '+')
    {
        RightBracketStatement();
    }
    ProgramBegin(data);
}


void commandStatement(char data[])
{
    int tempC = 0;
    char tempWord[MAX_TOKEN_LENGTH] = "";
    while(fullTokenList[globalCounter][tempC])
    {
        tempWord[tempC] = fullTokenList[globalCounter][tempC];
        tempC++;
    }

    if(strcmp(tempWord, "make") == 0)
    {
        globalCounter++;
    }
    else if(strcmp(tempWord, "go") == 0)
    {
        globalCounter++;
    }
    else
    {
        printf("\nPARSE ERROR(3): \"make/or\" IS EXPECTED BUT GIVEN \"%s\"\n\n", tempWord);
    }

    pathStatement(); //bunda neden data yok kontrol et
    EndOfLineStatement();
    ProgramBegin(data);
}

char * pathStatement(char data[])
{
    int tempC = 0;
    char *tempWord;
    tempWord = (char *) calloc(MAX_TOKEN_LENGTH, sizeof(char));
    while(fullTokenList[globalCounter][tempC])
    {
        tempWord[tempC] = fullTokenList[globalCounter][tempC];
        tempC++;
    }

    if(tempWord[0] == '<' && tempWord[tempC-1] == '>' && tempWord[1] != '>')
    {
        globalCounter++;
        if(data[0] == 'x') //data comes from ifStatement
        {
            return tempWord;
        }
        else   //data comes from commandStatement
        {
            //execute command
            printf("EXECUTE: %s %s\n", data, tempWord);
            //if command is make
            char tempDir[MAX_DIR_LENGTH] = "";
            if(strcmp(data, "make") == 0)
            {
                memset(TempCurrentDirectory,0,sizeof(TempCurrentDirectory));

                char storePath[MAX_TOKEN_LENGTH] = "";
                int storePathIndex = 0;
                int tempPathIndexNormal = 1;
                int firstTimeInit = 1;
                while(tempWord[tempPathIndexNormal])
                {
                    if(tempWord[tempPathIndexNormal] != '/' && tempWord[tempPathIndexNormal+1] != '\0')
                    {
                        storePath[storePathIndex++] = tempWord[tempPathIndexNormal];
                    }
                    else
                    {
                        if(firstTimeInit == 1 && tempDir[0] == '\0')
                        {
                            strcpy(tempDir, CurrentDirectory);
                            firstTimeInit = 0;
                        }

                        if(storePath[0] == '*')
                        {

                            if(tempDir[0] == '\0')
                            {
                                printf("\nWARNING: YOU CAN NOT GO UPPER THAN ROOT DIRECTORY\n\n");
                                break;
                            }
                            else
                            {
                                //remove one directory from tempCurrentDirectory
                                //temp yap cunku if false verirse currentDirectory degismesin(dokumanda yaziyor!)

                                int silmeCounter = strlen(tempDir) -1;
                                int slashCounter = 0;
                                while(slashCounter != 2)  //ters dolas
                                {
                                    if(tempDir[silmeCounter] == '/' || silmeCounter == -1)
                                    {
                                        slashCounter++;
                                    }
                                    else
                                    {
                                        tempDir[silmeCounter] = '\0';
                                    }
                                    silmeCounter--;
                                }
                            }
                        }
                        else
                        {
                            strcat(tempDir, storePath);
                            strcat(tempDir, "/");

                            int a = mkdir(tempDir);
                            if(a == -1){
                                printf("FILE WARNING: THE DIRECTORY \"%s\" IS ALREADY EXISTS\n",tempDir);
                            }

                            DIR* dir = opendir(tempDir);
                            if (dir)
                            {
                                // Directory exists.
                                closedir(dir);
                            }
                        }
                        memset(storePath,0,sizeof storePath);
                        storePathIndex=0;
                    }
                    tempPathIndexNormal++;
                }
            }
            else if(strcmp(data, "go") == 0)
            {
                char storePath[MAX_TOKEN_LENGTH] = "";
                int storePathIndex = 0;
                int tempPathIndexNormal = 1;
                int goPathFailed = 0; //dosya yoksa 1 olacak
                memset(TempCurrentDirectory, 0, sizeof(TempCurrentDirectory));
                strcat(TempCurrentDirectory, CurrentDirectory); //TempCurrentDirectory <= CurrentDirectory
                while(tempWord[tempPathIndexNormal])
                {
                    if(tempWord[tempPathIndexNormal] != '/' && tempWord[tempPathIndexNormal+1] != '\0')
                    {
                        storePath[storePathIndex++] = tempWord[tempPathIndexNormal];
                    }
                    else
                    {
                        if(storePath[0] == '*')
                        {

                            if(TempCurrentDirectory[0] == '\0')
                            {
                                printf("\nWARNING: YOU CAN NOT GO UPPER THAN ROOT DIRECTORY\n\n");
                                goPathFailed = 1;
                                break;
                            }
                            else
                            {
                                //remove one directory from tempCurrentDirectory
                                //temp yap cunku if false verirse currentDirectory degismesin(dokumanda yaziyor!)

                                int silmeCounter = strlen(TempCurrentDirectory) -1;
                                int slashCounter = 0;
                                while(slashCounter != 2)  //ters dolas
                                {
                                    if(TempCurrentDirectory[silmeCounter] == '/' || silmeCounter == -1)
                                    {
                                        slashCounter++;
                                    }
                                    else
                                    {
                                        TempCurrentDirectory[silmeCounter] = '\0';
                                    }
                                    silmeCounter--;
                                }
                            }
                        }

                        if(storePath[0] != '*')
                        {
                            strcat(TempCurrentDirectory, storePath);
                            strcat(TempCurrentDirectory, "/");
                        }

                        DIR* dir = opendir(TempCurrentDirectory);
                        if (dir)
                        {
                            // Directory exists.
                            //closedir(dir);
                        }
                        else
                        {
                            if(TempCurrentDirectory[0] == '\0')  //TempCurrentDirectory bos ise root dayiz ve hata verme
                            {
                                // Directory exists.
                            }
                            else
                            {
                                goPathFailed = 1;
                                printf("FILE ERROR: DIRECTORY \"%s\" IS NOT AVAILABLE!\n",TempCurrentDirectory);
                            }
                        }

                        memset(storePath,0,sizeof storePath);
                        storePathIndex=0;
                    }
                    tempPathIndexNormal++;
                }

                if(goPathFailed == 0)  //if path exist, then go to that path else stay same
                {
                    strcpy(CurrentDirectory, TempCurrentDirectory);
                }
            }
        }
    }
    else
    {
        printf("\nPARSE ERROR(4): \"Path Data Type\" IS EXPECTED BUT GIVEN \"%s\"\n\n", tempWord);
    }
    return "";
}

void EndOfLineStatement()
{
    int tempC = 0;
    char tempWord[MAX_TOKEN_LENGTH] = "";
    char tempWordType[MAX_TOKEN_LENGTH] = "";
    while(fullTokenList[globalCounter][tempC])
    {
        tempWord[tempC] = fullTokenList[globalCounter][tempC];
        tempC++;
    }
    tempC=0;
    while(fullTokenTypeList[globalCounter][tempC])
    {
        tempWordType[tempC] = fullTokenTypeList[globalCounter][tempC];
        tempC++;
    }

    if(strcmp(tempWordType, "EndOfLine") == 0)
    {
        globalCounter++;
    }
    else
    {
        printf("\nPARSE ERROR(5): \"EndOfLine\" IS EXPECTED BUT GIVEN \"%s\"\n\n", tempWord);
    }
}

void LeftBracketStatement()
{
    int tempC = 0;
    char tempWord[MAX_TOKEN_LENGTH] = "";
    char tempWordType[MAX_TOKEN_LENGTH] = "";
    while(fullTokenList[globalCounter][tempC])
    {
        tempWord[tempC] = fullTokenList[globalCounter][tempC];
        tempC++;
    }
    tempC=0;
    while(fullTokenTypeList[globalCounter][tempC])
    {
        tempWordType[tempC] = fullTokenTypeList[globalCounter][tempC];
        tempC++;
    }

    if(strcmp(tempWordType, "LeftCurlyBracket") == 0)
    {
        globalCounter++;
    }
    else
    {
        printf("\nPARSE ERROR(6): \"LeftCurlyBracket\" IS EXPECTED BUT GIVEN \"%s\"\n\n", tempWord);
    }
}

void RightBracketStatement()
{
    int tempC = 0;
    char tempWord[MAX_TOKEN_LENGTH] = "";
    char tempWordType[MAX_TOKEN_LENGTH] = "";
    while(fullTokenList[globalCounter][tempC])
    {
        tempWord[tempC] = fullTokenList[globalCounter][tempC];
        tempC++;
    }
    tempC=0;
    while(fullTokenTypeList[globalCounter][tempC])
    {
        tempWordType[tempC] = fullTokenTypeList[globalCounter][tempC];
        tempC++;
    }

    if(strcmp(tempWordType, "RightCurlyBracket") == 0)
    {
        globalCounter++;
    }
    else
    {
        printf("\nPARSE ERROR(7): \"RightCurlyBracket\" IS EXPECTED BUT GIVEN \"%s\"\n\n", tempWord);
    }
}


void isPath() // isPath(): Gelen cumle path data type ise 0 dan buyuk bir deger dondurur
{
    unsigned long long tempIndex = currentIndex;
    if(file[currentIndex] == '<')
    {
        while(file[tempIndex-1] != '>')
        {
            tempIndex++;
        }
        char *PathArray = (char *) calloc(tempIndex - currentIndex +1, sizeof(char));
        int i = 0;
        for(i = 0; currentIndex<tempIndex; i++)
        {
            //ilk karakter slash olamaz
            if(i == 1 && file[currentIndex] == '/')
            {
                printf("ERROR: PATH DATA TYPE CANNOT START WITH \"/\" SYMBOL!\n");
                exit(0);
            }
            //ilk karakter yildiz olabilir veya kendinden onceki karakter ve kendisi yildiz olabilir
            if((i == 1 && file[currentIndex] == '*') ||  (i>1 && PathArray[1] == '*' && PathArray[i-2] == '*' && file[currentIndex] == '*'))
            {
                PathArray[i] = file[currentIndex];
                currentIndex++;
                continue;
            }
            //valid operator mu kontrol et
            if(isalnum(file[currentIndex]) || file[currentIndex] == '<' || file[currentIndex] == '>' || file[currentIndex] == '/'  || file[currentIndex] == ' ' || file[currentIndex] == '_' )
            {

                if(isalpha(file[currentIndex]))
                {
                    file[currentIndex] = tolower(file[currentIndex]);
                }

                if(file[currentIndex] != ' ' )
                {
                    PathArray[i] = file[currentIndex];
                }
                else
                {
                    i--; //bosluk karakteri gelirse dizinin i. indisi null karakter kalmasin diye bi geri gidiyoruz
                }
            }
            else
            {
                printf("ERROR: INVALID OPERATOR IN PATH DATA TYPE: %c\n", file[currentIndex]);
                exit(0);
            }
            currentIndex++;
        }

        if(PathArray[i-2] == '/') //sondan 2 onceki karakter / ise hata ver
        {
            printf("ERROR: PATH DATA TYPE CANNOT END WITH \"/\" SYMBOL!\n");
            exit(0);
        }


        printf("Path Data Type Found: %s\n", PathArray);
        fprintf(sourceFileWrite, ".Path(%s)\n", PathArray);

        free(PathArray);
    }
}

void isCommand() // isCommand(): Gelen cumle command ise 0 dan buyuk bir deger dondurur
{
    unsigned long long tempIndex = currentIndex;
    if(isalpha(file[currentIndex]))
    {
        while(isalpha(file[tempIndex]))
        {
            tempIndex++;
        }
        char *CommandArray = (char *) calloc(tempIndex - currentIndex +1, sizeof(char));
        int i = 0;
        for(i = 0; currentIndex<tempIndex; i++)
        {
            if(isalpha(file[currentIndex]))
            {
                CommandArray[i] = file[currentIndex];
            }
            currentIndex++;
        }

        if(strcmp(CommandArray, "go") == 0 || strcmp(CommandArray, "make") == 0)
        {
            printf("Command Found: %s\n", CommandArray);
            fprintf(sourceFileWrite, ".Command(%s)\n", CommandArray);
        }
        else if(strcmp(CommandArray, "if") == 0 || strcmp(CommandArray, "ifnot") == 0)
        {
            printf("Control Structure Found: %s\n", CommandArray);
            fprintf(sourceFileWrite, ".ControlStructure(%s)\n", CommandArray);
        }
        else
        {
            printf("ERROR: %s IS NOT VALID COMMAND!\n", CommandArray);
            exit(0);
        }
        currentIndex--;
        free(CommandArray);
    }
}

void isOther() // isOther(): Gelen karakter suslu parantez, noktali virgul ise 0 dan buyuk bir deger dondurur
{
    if(file[currentIndex] == '{')
    {
        printf("Left Block Structure Found\n");
        fprintf(sourceFileWrite, "LeftCurlyBracket\n");
    }
    else if(file[currentIndex] == '}')
    {
        printf("Right Block Structure Found\n");
        fprintf(sourceFileWrite, "RightCurlyBracket\n");
    }
    else if(file[currentIndex] == ';')
    {
        printf("End Of Line Found\n");
        fprintf(sourceFileWrite, "EndOfLine\n");
    }
}
