#pragma once

#include "BaseTestTemplate.h"
#include "Properties.h"

#if defined(_WIN32) || defined(_WIN64)
    #define FROM_PATH "\\tmp\\"
#else
    #define FROM_PATH "/tmp/dir/"
#endif

#define PROPERTY_FILE_NAME "app.properties"

// https://en.wikipedia.org/wiki/.properties
char propertyBuffer[] = "# You are reading a comment in \".properties\" file.\n"
              "! The exclamation mark can also be used for comments.\n"
              "# Lines with \"properties\" contain a key and a value separated by a delimiting character.\n"
              "# There are 3 delimiting characters: '=' (equal), ':' (colon) and whitespace (space, \\t and \\f).\n"
              "website = https://en.wikipedia.org/\n"
              "language : \"English\"\r\n"
              "topic .properties files\n"
              "# A word on a line will just create a key with no value.\n"
              "empty\n"
              "# White space that appears between the key, the value and the delimiter is ignored.\n"
              "# This means that the following are equivalent (other than for readability).\n"
              "hello=hello\r\n"
              "hello = \"hello\"\n"
              "# Keys with the same name will be overwritten by the key that is the furthest in a file.\n"
              "# For example the final value for \"duplicateKey\" will be \"second\".\n"
              "duplicateKey = first\r\n"
              "duplicateKey = second\n"
              "# To use the delimiter characters inside a key, you need to escape them with a \\.\n"
              "# However, there is no need to do this in the value.\n"
              "delimiterCharacters\\:\\=\\ = This is the value for the key \"delimiterCharacters\\:\\=\\ \"\n"
              "# Adding a \\ at the end of a line means that the value continues to the next line.\n"
              "multiline = This line \\\n"
              "continues\n"
              "# If you want your value to include a \\, it should be escaped by another \\.\n"
              "path = c:\\\\wiki\\\\templates\n"
              "# This means that if the number of \\ at the end of the line is even, the next line is not included in the value.\n"
              "# In the following example, the value for \"evenKey\" is \"This is on one line\\\".\n"
              "evenKey = This is on one line\\\\\n"
              "# This line is a normal comment and is not included in the value for \"evenKey\"\n"
              "# If the number of \\ is odd, then the next line is included in the value.\n"
              "# In the following example, the value for \"oddKey\" is \"This is line one and\\#This is line two\".\n"
              "oddKey = This is line one and\\\\\\\n"
              "        # This is line two\n"
              "# White space characters are removed before each line.\n"
              "# Make sure to add your spaces before your \\ if you need them on the next line.\n"
              "# In the following example, the value for \"welcome\" is \"Welcome to Wikipedia!\".\n"
              "welcome = Welcome to \\\r\n"
              "          Wikipedia!\n"
              "# If you need to add newlines and carriage returns, they need to be escaped using \\n and \\r respectively.\n"
              "# You can also optionally escape tabs with \\t for readability purposes.\n"
              "valueWithEscapes=This is a newline\\n and a carriage return\\r and a tab\\t.\n";



static MunitResult testPropertiesLoad(const MunitParameter params[], void *data) {
    Properties *properties = LOAD_PROPERTIES_BUFF(propertyBuffer);
    assert_not_null(properties);
    assert_true(properties->status == CONFIG_PROP_OK);

    assert_uint32(propertiesSize(properties), ==, 13);
    assert_string_equal(getProperty(properties, "multiline"), "This line continues");
    assert_string_equal(getProperty(properties, "topic"), ".properties files");
    assert_string_equal(getProperty(properties, "oddKey"), "This is line one and# This is line two");
    assert_string_equal(getProperty(properties, "website"), "https://en.wikipedia.org/");
    assert_string_equal(getProperty(properties, "hello"), "hello");
    assert_string_equal(getProperty(properties, "duplicateKey"), "second");
    assert_string_equal(getProperty(properties, "delimiterCharacters\\:\\=\\"), "This is the value for the key \"delimiterCharacters\\:\\=\\");
    assert_string_equal(getProperty(properties, "valueWithEscapes"), "This is a newline\\n and a carriage return\\r and a tab\\t.");
    assert_string_equal(getProperty(properties, "welcome"), "Welcome to Wikipedia!");
    assert_string_equal(getProperty(properties, "path"), "c:\\\\wiki\\\\templates");
    assert_string_equal(getProperty(properties, "evenKey"), "This is on one line");
    assert_string_equal(getProperty(properties, "language"), "English");
    assert_string_equal(getPropertyOrDefault(properties, "err", "not found"), "not found");

    assert_null(getProperty(properties, "empty"));
    assert_true(propertiesHasKey(properties, "empty"));

    deleteConfigProperties(properties);

    return MUNIT_OK;
}

static MunitResult testPropertiesStore(const MunitParameter params[], void *data) {
    Properties *properties = INIT_PROPERTIES();
    assert_not_null(properties);
    assert_true(properties->status == CONFIG_PROP_OK);

    putProperty(properties, "k1", "v1");
    putProperty(properties, "k2", "v2");
    putProperty(properties, "k3", "v3");
    putProperty(properties, "k4", NULL);

    File *propFile = NEW_FILE(FROM_PATH PROPERTY_FILE_NAME);
    createFileDirs(propFile);
    storeProperties(properties, propFile->path);

    Properties *propsFromFile = LOAD_PROPERTIES(propFile->path);
    assert_not_null(propsFromFile);
    assert_true(propsFromFile->status == CONFIG_PROP_OK);

    assert_uint32(propertiesSize(propsFromFile), ==, 4);
    assert_string_equal(getProperty(propsFromFile, "k1"), "v1");
    assert_string_equal(getProperty(propsFromFile, "k2"), "v2");
    assert_string_equal(getProperty(propsFromFile, "k3"), "v3");

    assert_null(getProperty(propsFromFile, "k4"));
    assert_true(propertiesHasKey(propsFromFile, "k4"));

    // Invalid properties
    Properties *errorProps = LOAD_PROPERTIES(NULL);
    assert_true(errorProps->status == CONFIG_PROP_ERROR_INVALID_FILE_TYPE);
    assert_null(errorProps->map);

    errorProps = LOAD_PROPERTIES("props.log");  // invalid file extension
    assert_true(errorProps->status == CONFIG_PROP_ERROR_INVALID_FILE_TYPE);
    assert_null(errorProps->map);

    deleteConfigProperties(properties);
    deleteConfigProperties(propsFromFile);
    remove(propFile->path);

    return MUNIT_OK;
}

static MunitResult testPropertiesToStr(const MunitParameter params[], void *data) {
    Properties *properties = INIT_PROPERTIES();
    assert_not_null(properties);
    assert_true(properties->status == CONFIG_PROP_OK);

    putProperty(properties, "k1", "v1");
    putProperty(properties, "k2", "v2");
    putProperty(properties, "k3", "v3");
    putProperty(properties, "k4", NULL);

    char buffer[128] = {0};
    propertiesToString(properties, buffer, 128);

    assert_not_null(strstr(buffer, "[k1]=[v1]"));
    assert_not_null(strstr(buffer, "[k2]=[v2]"));
    assert_not_null(strstr(buffer, "[k3]=[v3]"));
    assert_not_null(strstr(buffer, "[k4]"));

    deleteConfigProperties(properties);

    return MUNIT_OK;
}


static MunitTest propertiesTests[] = {
        {.name =  "Test loadPropertiesBuffer() - should correctly parse properties", .test = testPropertiesLoad},
        {.name =  "Test storeProperties() - should correctly save properties", .test = testPropertiesStore},
        {.name =  "Test propStatusToString() - should correctly map properties to string", .test = testPropertiesToStr},
        END_OF_TESTS
};

static const MunitSuite propertiesTestSuite = {
        .prefix = "Properties: ",
        .tests = propertiesTests,
        .suites = NULL,
        .iterations = 1,
        .options = MUNIT_SUITE_OPTION_NONE
};
