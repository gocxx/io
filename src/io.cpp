#include "gocxx/io/io.h"
#include <memory>
#include <vector>

namespace gocxx::io {

    IOResult Copy(std::shared_ptr<Writer> dest, std::shared_ptr<Reader> source) {
        constexpr std::size_t bufferSize = 8192;
        std::vector<uint8_t> buffer(bufferSize);

        std::size_t totalBytes = 0;

        while (true) {
            IOResult readResult = source->read(buffer.data(), bufferSize);

            if (!readResult.ok()) {
                if (readResult.eof()) {
                    break;
                }
                return { totalBytes, readResult.error, readResult.message };
            }

            std::size_t bytesRead = readResult.bytesTransferred;

            IOResult writeResult = dest->write(buffer.data(), bytesRead);
            if (!writeResult.ok()) {
                return { totalBytes, writeResult.error, writeResult.message };
            }

            totalBytes += writeResult.bytesTransferred;
        }

        return { totalBytes, IOError::None };
    }

     // --- MemoryReader ---

    MemoryReader::MemoryReader(const std::vector<uint8_t>& d)
        : data(d) {}

    IOResult MemoryReader::read(uint8_t* buffer, std::size_t size) {
        if (pos >= data.size()) {
            return {0, IOError::EOFReached};
        }

        std::size_t remaining = data.size() - pos;
        std::size_t toRead = std::min(size, remaining);

        std::memcpy(buffer, data.data() + pos, toRead);
        pos += toRead;

        return {toRead, IOError::None};
    }

    // --- MemoryWriter ---

    MemoryWriter::MemoryWriter() = default;

    IOResult MemoryWriter::write(const uint8_t* buffer, std::size_t size) {
        if (!buffer || size == 0) {
            return {0, IOError::Unknown,"buffer pointer is null / zero buffer"};
        }

        data.insert(data.end(), buffer, buffer + size);
        return {size, IOError::None};
    }

    const std::vector<uint8_t>& MemoryWriter::getData() const {
        return data;
    }

    // --- FileReader ---

    FileReader::FileReader(FILE* file) {
        this->file = file;
    }


    IOResult FileReader::read(uint8_t* buffer, std::size_t size) {
        if (!file || !buffer || size == 0) {
            return {0, IOError::ReadError};
        }

        std::size_t bytes = std::fread(buffer, 1, size, file);
        if (bytes == 0) {
            if (std::feof(file)) {
                return {0, IOError::EOFReached};
            }
            return {0, IOError::ReadError};
        }

        return {bytes, IOError::None};
    }

    // --- FileWriter ---

    FileWriter::FileWriter(FILE* file) {
		this->file = file;
    }

    IOResult FileWriter::write(const uint8_t* buffer, std::size_t size) {
        if (!file || !buffer || size == 0) {
            return {0, IOError::WriteError};
        }

        std::size_t written = std::fwrite(buffer, 1, size, file);
        if (written != size) {
            return {written, IOError::WriteError};
        }

        return {written, IOError::None};
    }

	// --- IStreamReader ---

    IStreamReader::IStreamReader(std::istream& inStream)
        : in(inStream) {
    }

    IOResult IStreamReader::read(uint8_t* buffer, std::size_t size) {
        if (!buffer || size == 0) {
            return { 0, IOError::ReadError, "Invalid buffer or size" };
        }

        in.read(reinterpret_cast<char*>(buffer), size);
        std::size_t bytesRead = static_cast<std::size_t>(in.gcount());

        if (in.bad()) {
            return { bytesRead, IOError::ReadError, "Stream read failed" };
        }
        else if (in.eof()) {
            return { bytesRead, IOError::EOFReached };
        }

        return { bytesRead, IOError::None };
    }

    OStreamWriter::OStreamWriter(std::ostream& outStream)
        : out(outStream) {
    }

    IOResult OStreamWriter::write(const uint8_t* buffer, std::size_t size) {
        if (!buffer || size == 0) {
            return { 0, IOError::WriteError, "Invalid buffer or size" };
        }

        out.write(reinterpret_cast<const char*>(buffer), size);

        if (!out) {
            return { 0, IOError::WriteError, "Stream write failed" };
        }

        return { size, IOError::None };
    }

} // namespace io
