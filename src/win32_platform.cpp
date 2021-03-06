#include "win32_platform.h"

/* functions related to windows specific platform */
bool WindowSetup(SDL_Window **mainWindow, const char *programName)
{
    // Initialize SDL's video subsystem
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "Failed to init SDL\n";
        return false;
    }

    *mainWindow = SDL_CreateWindow(programName, SDL_WINDOWPOS_CENTERED, // NOLINT
                                   SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH,        // NOLINT
                                   SCREEN_HEIGHT, SDL_WINDOW_OPENGL);

    if (!*mainWindow)
    {
        std::cout << "Unable to create window\n";
        CheckSDLError(__LINE__);
        return false;
    }

    return true;
}

bool WindowsSDLTTFSetup()
{
    if (TTF_Init() < 0)
    {
        printf("TTF failed to init\n");
        return false;
    }

    printf("TTF init complete\n");

    return true;
}

bool WindowsOpenGLSetup(SDL_Window *mainWindow, SDL_GLContext *mainContext)
{
#if 0
    //_setOpenGLSettings();
    /* create our opengl context and attach it to our window */
    *mainContext = SDL_GL_CreateContext(mainWindow);

    /* initialize to start using opengl */
    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();

    GLenum err;
    if ((err = glGetError()) != GL_NO_ERROR)
    {
        /* Ignore GL_INVALID_ENUM. There are cases where using glewExperimental
         * can cause a GL_INVALID_ENUM, which is fine -- just ignore it.
         * Otherwise, we do have a problem.
         */
        if (err != GL_INVALID_ENUM)
        {
            printf("OpenGL: found true error x%x\n", err);
        }
    }

    if (glewError != GLEW_OK)
    {
        std::cout << "Error initializing GLEW! "
                  << glewGetErrorString(glewError) << std::endl;
        //return false;
    }

    if (SDL_GL_SetSwapInterval(1) < 0)
    {
        printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
    }
#endif

    return true;
}

void _setOpenGLSettings()
{
    // Set our OpenGL version.
    // SDL_GL_CONTEXT_CORE gives us only the newer version, deprecated functions
    // are disabled
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    //SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    // Turn on double buffering with a 24bit Z buffer.
    // You may need to change this to 16 or 32 for your system
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // This makes our buffer swap syncronized with the monitor's vertical
    // refresh
    SDL_GL_SetSwapInterval(1);
}

void CheckSDLError(int /*line*/)
{
    std::string error = SDL_GetError();

    /* TODO: Do something here */
    SDL_ClearError();
}

void WindowsCleanup(SDL_Window *mainWindow, SDL_GLContext *mainContext)
{
    SDL_GL_DeleteContext(mainContext);
    SDL_DestroyWindow(mainWindow);
    TTF_Quit();
    SDL_Quit();
}

bool LoadDLLWindows(RenderAPI *renderAPI)
{
    renderAPI->libHandle = LoadLibrary("update_and_render.dll");
    if (!renderAPI->libHandle)
    {
        printf("Failed to load library! \n");
        return false;
    }

    HMODULE RenderDLL = renderAPI->libHandle;
    renderAPI->updateAndRender =
        (UPDATEANDRENDER)GetProcAddress(RenderDLL, "UpdateAndRender");

    if (!renderAPI->updateAndRender)
    {
        printf("Failed to load function \"UpdateAndRender\"!\n");
        return false;
    }
    return true;
}

/* HELPER FUNCTIONS */

void PrintSDLTTFVersion()
{
    const SDL_version *linkedVersion = TTF_Linked_Version();
    SDL_version compiledVersion;
    SDL_TTF_VERSION(&compiledVersion);

    std::cout << "Linked version:\n"
              << linkedVersion->major << "." << linkedVersion->minor << "."
              << linkedVersion->patch;

    std::cout << "Compiled version:\n"
              << compiledVersion.major << "." << compiledVersion.minor << "."
              << compiledVersion.patch << std::endl;
}

char *GetProgramPath()
{
    char *dataPath = nullptr;
    char *basePath = SDL_GetBasePath();
    if (basePath)
    {
        dataPath = basePath;
    }
    else
    {
        dataPath = SDL_strdup("./");
    }
    return dataPath;
}

void PrintFileTimeStamp(WIN32_FIND_DATA searchData)
{
    SYSTEMTIME stUTC, stLocal;

    /* Convert the last-write time to local time. */
    FileTimeToSystemTime(&searchData.ftCreationTime, &stUTC);
    SystemTimeToTzSpecificLocalTime(nullptr, &stUTC, &stLocal);

    printf("%02d/%02d/%d  %02d:%02d\n", stLocal.wMonth, stLocal.wDay,
           stLocal.wYear, stLocal.wHour, stLocal.wMinute);
}

void FindFile(const char *dirPath, const char *fileRegex)
{
    const unsigned BUFFER_SIZE = 256;
    char searchPath[BUFFER_SIZE];

    strncpy_s(searchPath, dirPath, BUFFER_SIZE - 1);
    strncat_s(searchPath, fileRegex, BUFFER_SIZE - strlen(searchPath) - 1);

    WIN32_FIND_DATA searchData;
    memset(&searchData, 0, sizeof(WIN32_FIND_DATA));

    HANDLE handle = FindFirstFile(searchPath, &searchData);

    while (handle != INVALID_HANDLE_VALUE)
    {

#if 0
        unsigned long qwResult =
            (((ULONGLONG)searchData.ftLastWriteTime.dwHighDateTime) << 32) +
            searchData.ftLastWriteTime.dwLowDateTime;
#endif

        if (FindNextFile(handle, &searchData) == FALSE)
        {
            break;
        }
    }

    /* Close the handle after use or memory/resource leak */
    FindClose(handle);
}

void GetLatestFile()
{
}

void ListFiles(const char *dirPath)
{
    const unsigned BUFFER_SIZE = 256;
    const char *fileRegex = "*";
    char searchPath[BUFFER_SIZE];

    strncpy_s(searchPath, dirPath, BUFFER_SIZE - 1);
    strncat_s(searchPath, fileRegex, BUFFER_SIZE - strlen(searchPath) - 1);

    WIN32_FIND_DATA search_data;
    memset(&search_data, 0, sizeof(WIN32_FIND_DATA));

    HANDLE handle = FindFirstFile(searchPath, &search_data);
    while (handle != INVALID_HANDLE_VALUE)
    {
        if (FindNextFile(handle, &search_data) == FALSE)
        {
            break;
        }
    }

    /* Close the handle after use or memory/resource leak */
    FindClose(handle);
}
