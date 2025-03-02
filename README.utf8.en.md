# Logger

Logger is a policy based logger. That means that you functionality depends on policies that would be passed to logger as template parameters.

## License

BSD-3 clause

see `License.en.utf8.txt`/`License.ru.utf8.txt`

## Version

`1.0`

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

## Custom policies

You could use your own policies or you own custom implementation of policies. The only requirements is to satisfy `logger::LoggerPolicy` concept:

```cpp
template<class T>
concept LoggerPolicy = requires (std::string_view message)
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

## Concepts

There is come concepts to simplify some checks:

- `LoggerPolicy<T>` check if `T` is a policy type (see above)

- `HasLevels<T>` check if `T` has logging levels enumerate like
  
  - `T::Level::DEBUG`
  
  - `T::Level::INFO`
  
  - `T::Level::WARNING`
  
  - `T::Level::ERROR`

- `IsLogger<T>` check if `T` according to the logger type has next functions:
  
  - `void log(T::Level, std:;string_view) const`
  
  - `void debug(std::string_view) const` 
  
  - `void info(std::string_view) const`
  
  - `void warning(std::string_view) const`
  
  - `void error(std::string_view) const`

- `LoggerType<T>` is the same as `IsLogger<T>` (to use in template expressions)

- `HasPolicy<T, P>` check if `T` is `LoggerType` and `T` has policy `P` for example:
  
  ```cpp
  using Logger = logger::Logger<logger::DefaultFileLoggerPolicy,
                                logger::DefaultConsoleLoggerPolicy>;
  static_assert(logger::HasPolicy<Logger, logger::DefaultFileLoggerPolicy>);
  ```

- `HasNoPolicy<T, P>` reversed for `HasPolicy<T, P>`
