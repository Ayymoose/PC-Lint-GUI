#ifndef JENKINS_H
#define JENKINS_H

#define APPLICATION_NAME "Linty"
#define BUILD_VERSION "Jenkins build replace"
#define BUILD_DATE "Jenkins build replace"
#define BUILD_CC VER_STRING(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);

#define STRINGIFY(x) #x
#define VER_STRING(major, minor, patch) (STRINGIFY(major) "." STRINGIFY(minor) "." STRINGIFY(patch))

#endif // JENKINS_H
