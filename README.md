# Logger

Logger is a policy based thread safe logger. That means that you functionality depends on policies that would be passed to logger as template parameters.

## License

[BSD-3 clause](License.utf8.en.txt)

## Dependencies

- **rapidjson** - [site](https://rapidjson.org/); [license](src/thirdparty/rapidjson/license.txt); version - 1.1.0
- **googletest** - [site](https://google.github.io/googletest); [license](src/thirdparty/googletest/LICENSE); version - 1.16.0

## Version

`1.5.0`

## Why Policy-Based Design?

The policy-based design pattern was chosen for this logger because it offers high flexibility, compile-time safety, and performance benefits. Here’s why:

- easily swap logging policies without modifying the core Logger<> class;

- allows logging to multiple outputs (console, file, network, database) by simply passing different policies as template parameters;

- users can define their own policies without modifying existing code;

- compile-time safety - the use of C++20 Concepts ensures that only valid policies can be used;

- zero runtime overhead - unlike virtual functions (which introduce vtable lookups), policies are resolved at compile time, leading to zero runtime overhead;

- since the logger is fully templated, the compiler inlines function calls, making it as fast as possible;

- multiple policies at once - unlike traditional inheritance, multiple policies can be combined to allow logging to different backends simultaneously; no need for complex inheritance trees - just pass multiple policies to `Logger<>`.

## Basic usage

### Console only logging

```cpp
using Logger = logger::Logger<logger::DefaultConsoleLoggerPolicy>;

void foo()
{
    Logger logger;

    logger.log(Logger::Level::DEBUG, debug_message);
    logger.log(Logger::Level::INFO, info_message);
    logger.log(Logger::Level::WARNING, warning_message);
    logger.log(Logger::Level::ERROR, error_message);

    logger.debug(debug_message);
    logger.info(info_message);
    logger.warning(warning_message);
    logger.error(error_message);
}
```

### File only logging

```cpp
using Logger = logger::Logger<logger::DefaultFileLoggerPolicy>;

void foo()
{
    logger::DefaultFileLoggerPolicy::set_file_path("log.log");

    Logger logger;

    logger.log(Logger::Level::DEBUG, debug_message);
    logger.log(Logger::Level::INFO, info_message);
    logger.log(Logger::Level::WARNING, warning_message);
    logger.log(Logger::Level::ERROR, error_message);

    logger.debug(debug_message);
    logger.info(info_message);
    logger.warning(warning_message);
    logger.error(error_message);
}
```

### Both type the same time

```cpp
using Logger = logger::Logger<logger::DefaultFileLoggerPolicy,
                              logger::DefaultConsoleLoggerPolicy>;

void foo()
{
    logger::DefaultFileLoggerPolicy::set_file_path("log.log");

    Logger logger;

    logger.log(Logger::Level::DEBUG, debug_message);
    logger.log(Logger::Level::INFO, info_message);
    logger.log(Logger::Level::WARNING, warning_message);
    logger.log(Logger::Level::ERROR, error_message);

    logger.debug(debug_message);
    logger.info(info_message);
    logger.warning(warning_message);
    logger.error(error_message);
}
```

### Initialized/Releasable policies

Logger has concepts of initialized and releasable policies (see concepts `InitializedPolicy<T>` and `ReleasablePolicy<T>`) to initialize policy by itself. Policies could be the same time initialized and releasable, or not. Logger will call `init()` for all policies that satisfy `InitializedPolicy<T>` concept and call `release()` for all policies that satisfy `ReleasablePolicy<T>` concept. For example:

```cpp
struct SomeInitializedPolicy
{
    static void init(void)
    {
        // some initialization
    }

    static void write(std::string_view message)
    {
        std::cout << message << std::endl;
    }
};
static_assert(initialized_policy<SomeInitializedPolicy>);


struct SomeReleasablePolicy
{
    static void release(void)
    {
        // some release
    }

    static void write(std::string_view message)
    {
        std::cout << message << std::endl;
    }
};
static_assert(logger::releasable_policy<SomeReleasablePolicy>);

struct SomeInitializedAndReleasablePolicy
{
    static void init(void)
    {
        // some initialization
    }

    static void release(void)
    {
        // some release
    }

    static void write(std::string_view message)
    {
        std::cout << message << std::endl;
    }
};
static_assert(logger::initialized_policy<SomeInitializedAndReleasablePolicy>);
static_assert(logger::releasable_policy<SomeInitializedAndReleasablePolicy>);

using logger_t = logger::Logger<SomeInitializedPolicy,
                                SomeReleasablePolicy,
                                SomeInitializedAndReleasablePolicy>;

void foo()
{
    logger_t log(); // call of SomeInitializedPolicy::init()
                    // and SomeInitializedAndReleasablePolicy::init()

    log.debug("some debug message");
} // call if SomeReleasablePolicy::release() and
  // SomeInitializedAndReleasablePolicy::release() in the end of scope
```

## Custom policies

You could use your own policies or you own custom implementation of policies. The only requirements is to satisfy `logger::LoggerPolicy` concept:

```cpp
template<class T>
concept logger_policy = requires (std::string_view message)
{
    { T::write(message) };
};
```

That means that you need to implement `static void write(std::string_view message)` function in your policy.

For example:

```cpp
struct CustomConsoleLoggerPolicy
{
    static void write(std::string_view message)
    {
        std::cout << "[CUSTOM] " << message << std::endl;
    }
};
```

Then you can use:

```cpp
using Logger = logger::Logger<CustomConsoleLoggerPolicy>;
```

## Important warning about using `initialized_policy` and `releasable_policy`

There is some collision with using of the same policies in several different logger instances if they implement `initialized_policy` and `releasable_policy` concepts.
For example this code leads to a double freeing and initialization for `FilePolicy`:

```cpp
struct FilePolicy
{
    static void init(){}
    static void write(std::string_view message){}
    static void release(){}
};

struct SocketPolicy
{
    static void init(){}
    static void write(std::string_view message){}
    static void release(){}
};

static_assert(logger::initialized_policy<FilePolicy>);
static_assert(logger::releasable_policy<FilePolicy>);
static_assert(logger::initialized_policy<SocketPolicy>);
static_assert(logger::releasable_policy<SocketPolicy>);

using FileLogger = logger::Logger<FilePolicy>;
using FileSocketLogger = logger::Logger<FilePolicy, SocketPolicy>;

int main()
{
    FileLogger f_logger; // call FilePolicy::init() for FileLogger 
    FileSocketLogger  fs_logger; // call FilePolicy::init() and
                                 // SocketPolicy::init() for
                                 // FileSocketLogger
} // call FilePolicy::release() twice
  // (once from FileLogger and once more time for FileSocketLogger)
  // and SocketPolicy::release once
```

There is could be two decisions:

1) Do not use the same policy for different logger

2) If you really need to use the same policies for different loggers - bypass implementation of `initialized_policy` and `releasable_policy`. Then initialization and releasing moments of policies is your responsibility.

## Concepts

There is come concepts to simplify some checks:

- `logger_policy<T>` check if `T` is a policy type (see above)

- `initialized_policy<T>` check if `T` is initialized policy - that is, it is a policy type and has static function `void init(void)`

- `releasable_policy<T>` check if `T` is releasable policy - that is, it is a policy type and has static function `void release(void)`

- `has_levels<T>` check if `T` has logging levels enumerate like
  
  - `T::Level::DEBUG`
  
  - `T::Level::INFO`
  
  - `T::Level::WARNING`
  
  - `T::Level::ERROR`

- `is_logger<T>` check if `T` according to the logger type has next functions:
  
  - `void log(T::Level, std:;string_view) const`
  
  - `void debug(std::string_view) const` 
  
  - `void info(std::string_view) const`
  
  - `void warning(std::string_view) const`
  
  - `void error(std::string_view) const`

- `logger_type<T>` is the same as `is_logger<T>` (to use in template expressions)

- `is_polisy_in_list<Policy, class... Policies>` check if `Policy` in `Policies` list

- `has_policy<Policy, class... Policies>` the same as `is_polisy_in_list`

- `logger_has_policy<T, P>` check if `T` is `LoggerType` and `T` has policy `P` for example:
  
  ```cpp
  using Logger = logger::Logger<logger::DefaultFileLoggerPolicy,
                                logger::DefaultConsoleLoggerPolicy>;
  static_assert(logger::HasPolicy<Logger, logger::DefaultFileLoggerPolicy>);
  ```

- `logger_has_no_policy<T, P>` reversed for `LoggerHasPolicy<T, P>`

## Configuration

Logger can read json config file, and it can be the same config a single configuration file with your project. The main point is to has `logger` key in the main level of json config (or, simplify, one of the following samples):

1. ```json
   {
       "logger" : {
           "log_file" : "/path/to/logfile",
           "log_level" : "debug",
           "log_pattern" : "[{{time}}][{{level}}] {{message}}"
       },
       "other_config" : {
           // some other config
       }
   }
   ```

2. ```json
   {
       "logger" : {
           "log_file" : "/path/to/logfile",
           "log_level" : "debug",
           "log_pattern" : "[{{time}}][{{level}}] {{message}}"
       }
   }
   ```
- Use `logger::read_config(const std::filesystem::path& file)` to read configuration from specified file

- Use `logger::read_config_from_json(const std::string& json_text)` to read configuration from json text

### Configuration items

- **log_file** - output file where output log will be placed

- **log_level** - minimal output level that will be written

- **log_pattern** - log message pattern according to that log will write messages;
  supports the next items:
  '*{{time}}*' - time of the message in the format: YYYY-mm-DD HH:MM:SS.ms UTC_TIMEZONE (the format actually depends on *TimeProvider* that you can provide in DI - see the relevant paragraph)
  '*{{thread-id}}*' - id of the current thread
  '*{{level}}*' - log level: debug, info, warning, error
  '*{{message}}*' - output message

## Dependencies container (DI)

There is an approach for customizing some behavior of logger with *DependencyContainer* class. By default there is defaults providers.

usage:

```cpp
int main()
{
    logger::DependencyContainer::emplace<logger::TimeProvider, MyTimeProvider>(my_time_provider_ctor_args);
    logger::DependencyContainer::set<logger::TimeProvider>(std::make_shared<MyTimeProvider>(my_time_provider_ctor_args));
    auto time_provider = logger::DependencyContainer::get<logger::TimeProvider>();
    // ...
}
```

### Available providers:

*for all of these dependencies default implementations provided and these initialization is hidden from users: that means that you can't control that dependencies initialization.

#### *TimeProvider*

provider for getting current time as string. Should implement the next interface (to be the derivative from):   

```cpp
struct TimeProvider
{
    virtual ~TimeProvider() = default;
    virtual std::string now() const = 0;
};
```
