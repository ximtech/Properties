# Properties

Property files similar to `.ini` files mainly used to store the configurable parameters of an application.
They can also store different parameters as a pair of strings, one storing the name of parameter(key) and the
other storing the value.

More info on wiki: [link](https://en.wikipedia.org/wiki/.properties)

### Dependencies

- [FileUtils](https://github.com/ximtech/FileUtils)

### Features

- Properties load from file
- Properties load from char buffer
- Edit/Store properties in file
- Lightweight design
- Single include
- Easy to use
-

### Tradeoffs

- Dynamic memory allocation used for each key and value
- External dependencies

### Add as CPM project dependency

How to add CPM to the project, check the [link](https://github.com/cpm-cmake/CPM.cmake)

```cmake
CPMAddPackage(
        NAME Properties
        GITHUB_REPOSITORY ximtech/Properties
        GIT_TAG origin/main)

target_link_libraries(${PROJECT_NAME} Properties)
```

```cmake
add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})
# For Clion STM32 plugin generated Cmake use 
target_link_libraries(${PROJECT_NAME}.elf Properties)
```

## Basic format

Property files `.properties` are plain text files, with rows with line breaks and each row contains `key` and `values`

```properties
# There are 3 delimiting characters: '=' (equal), ':' (colon) and whitespace ' '.
key1=value1
key2  =value 2
key3:value3
key4 value4
```

### Comments

```properties
# You are reading a comment in ".properties" file.
! The exclamation mark can also be used for comments.
```

### Multiline values

```properties
# Adding a \ at the end of a line means that the value continues to the next line.
multiline = This line \
            continues \
            here
```

### Usage

### Single header include

```c
#include "Properties.h"
```

### Load properties from a file

***NOTE:*** File extension should be `.properties`

#### Property file `config.properties` example

```properties
# last modified 1 April 2001 by John Doe
name = John Doe
organization = Very \
              long \
              organization \
              name

# use IP address in case network name resolution is not working
server = 192.0.2.62
port = 143
file = "payroll.dat"

```

```c
Properties *properties = LOAD_PROPERTIES("config.properties");
if (properties->status != CONFIG_PROP_OK) {
    printf("[%s]\n", propStatusToString(properties->status));   // print error status message
    exit(EXIT_FAILURE);
}

char *name = getProperty(properties, "name");   // John Doe
char *organization = getProperty(properties, "organization");   // Very long organization name
char *defaultValue = getPropertyOrDefault(properties, "not.existing", "optional"); // optional

assert(propertiesSize(properties) == 5);
char *server = propertiesRemove(properties, "server");  // 192.0.2.62
printf("Name: [%s], Org: [%s], Default: [%s], Server: [%s]\n", name, organization, defaultValue, server);

assert(propertiesSize(properties) == 4);    // key "server" removed from map
assert(isEmptyProperties(properties) == false);
assert(propertiesHasKey(properties, "port") == true);
assert(propertiesHasKey(properties, "server") == false);
```

### Load properties from a char buffer

```c
char buffer[] = "key1=value1\n"
                "key2=value 2\n"
                "key3:value3\n"
                "key4 value4\n";

Properties properties;
loadPropertiesBuffer(&properties, buffer);
if (properties.status != CONFIG_PROP_OK) {
    printf("[%s]\n", propStatusToString(properties.status));   // print error status message
    exit(EXIT_FAILURE);
}
```
#### Short version
```c
Properties *properties = LOAD_PROPERTIES_BUFF(buffer);
```

### Save properties to file

***NOTE:*** For already existing file all data will be rewritten

```c
Properties properties = {0};
initProperties(&properties);

putProperty(&properties, "k1", "v1");
putProperty(&properties, "k2", "v2");
putProperty(&properties, "k3", "v3");
putProperty(&properties, "k4", NULL);

storeProperties(&properties, "config.properties");
```

#### Short version

```c
Properties *properties = INIT_PROPERTIES();
```

### Properties to string

```c
Properties *properties = INIT_PROPERTIES();
putProperty(properties, "k1", "v1");
putProperty(properties, "k2", "v2");

char buffer[32] = {0};
propertiesToString(properties, buffer, 512);

printf("%s", buffer);
```

#### Output
```text
[k2]=[v2]
[k1]=[v1]
```

### Delete properties

Free all resources
```c
deleteConfigProperties(properties);
```



















