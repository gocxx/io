#pragma once

#include <cstdint>
#include <cstddef>
#include <optional>
#include <string>
#include <memory>
#include <vector>
#include <cstdio>
#include <istream>

namespace gocxx::io {
    enum class IOError
    {
        None = 0,
        EOFReached,
        ReadError,
        WriteError,
        Timeout,
        Interrupted,
        Unknown
    };

    inline const char* toString(IOError err) {
        switch (err) {
        case IOError::None:        return "No error";
        case IOError::EOFReached:  return "End of file";
        case IOError::ReadError:   return "Read error";
        case IOError::WriteError:  return "Write error";
        case IOError::Timeout:     return "Timeout";
        case IOError::Interrupted: return "Interrupted";
        case IOError::Unknown:     return "Unknown I/O error";
        default:                   return "Unrecognized I/O error";
        }
    }

    struct IOResult
    {
        std::size_t bytesTransferred = 0;
        IOError error = IOError::None;
        std::optional<std::string> message;

        bool ok() const { return error == IOError::None; }
        bool eof() const { return error == IOError::EOFReached; }

        std::string error_message() const
        {
            if (message && !message->empty())
            {
                return message.value_or("error") + " : " + toString(error);
            }
            return toString(error);
        }
    };

    class Reader
    {
    public:
        virtual ~Reader() = default;

        // Reads up to `size` bytes into `buffer`.
        // Returns the number of bytes read (0 = EOF).
        virtual IOResult read(uint8_t* buffer, std::size_t size) = 0;
    };

    class Writer
    {
    public:
        virtual ~Writer() = default;

        // Writes `size` bytes from `buffer`.
        // Returns the number of bytes written (0 = error).
        virtual IOResult write(const uint8_t* buffer, std::size_t size) = 0;
    };

    IOResult Copy(std::shared_ptr<Writer> dest, std::shared_ptr<Reader> source);



    class MemoryReader : public io::Reader {
    public:
        explicit MemoryReader(const std::vector<uint8_t>& d);
        io::IOResult read(uint8_t* buffer, std::size_t size) override;

    private:
        std::vector<uint8_t> data;
        std::size_t pos = 0;
    };

    class MemoryWriter : public io::Writer {
    public:
        MemoryWriter();
        io::IOResult write(const uint8_t* buffer, std::size_t size) override;
        const std::vector<uint8_t>& getData() const;

    private:
        std::vector<uint8_t> data;
    };

    class FileReader : public io::Reader {
    public:
        explicit FileReader(FILE* file);
        ~FileReader() = default;
        io::IOResult read(uint8_t* buffer, std::size_t size) override;

    private:
        FILE* file = nullptr;
    };

    class FileWriter : public io::Writer {
    public:
        explicit FileWriter(FILE* file);
        ~FileWriter() = default;
        io::IOResult write(const uint8_t* buffer, std::size_t size) override;

    private:
        FILE* file = nullptr;
    };

    class IStreamReader : public io::Reader {
    public:
        explicit IStreamReader(std::istream& in);
        io::IOResult read(uint8_t* buffer, std::size_t size) override;

    private:
        std::istream& in;
    };

    class OStreamWriter : public io::Writer {
    public:
        explicit OStreamWriter(std::ostream& out);
        io::IOResult write(const uint8_t* buffer, std::size_t size) override;

    private:
        std::ostream& out;
    };


}; // namespace io
