
#include <gtest/gtest.h>

#include "gocxx/io/io.h"

#include <iostream>

using namespace gocxx::io;

TEST(MemoryReaderWriterTest, BasicWriteAndRead) {
    std::vector<uint8_t> inputData = { 'H', 'e', 'l', 'l', 'o' };

    MemoryWriter writer;
    auto writeResult = writer.write(inputData.data(), inputData.size());

    ASSERT_TRUE(writeResult.ok());
    ASSERT_EQ(writeResult.bytesTransferred, inputData.size());

    MemoryReader reader(writer.getData());
    std::vector<uint8_t> outputBuffer(5);
    auto readResult = reader.read(outputBuffer.data(), outputBuffer.size());

    ASSERT_TRUE(readResult.ok());
    ASSERT_EQ(readResult.bytesTransferred, 5);
    EXPECT_EQ(outputBuffer, inputData);
}

TEST(MemoryReaderTest, EOFHandled) {
    std::vector<uint8_t> data = { 'X' };
    MemoryReader reader(data);
    std::vector<uint8_t> buffer(2);

    auto first = reader.read(buffer.data(), 1);
    ASSERT_TRUE(first.ok());
    ASSERT_EQ(first.bytesTransferred, 1);

    auto second = reader.read(buffer.data(), 1);
    ASSERT_TRUE(second.eof());
    ASSERT_EQ(second.bytesTransferred, 0);
}

TEST(IOCopyTest, CopyFromMemoryToMemory) {
    std::vector<uint8_t> input = { 'A', 'B', 'C', 'D' };
    auto reader = std::make_unique<MemoryReader>(input);
    auto writer = std::make_unique<MemoryWriter>();

    auto result = Copy(std::move(writer), std::move(reader));
    ASSERT_TRUE(result.ok());
    ASSERT_EQ(result.bytesTransferred, input.size());
}

TEST(FileReaderWriterTest, WriteThenReadBack) {
    std::string filename = "test_output.tmp";
    std::vector<uint8_t> content = { '1', '2', '3', '4' };

    {
		FILE* file = std::fopen(filename.c_str(), "wb");
        FileWriter writer(file);
        auto result = writer.write(content.data(), content.size());
		std::fclose(file);
        ASSERT_TRUE(result.ok());
        ASSERT_EQ(result.bytesTransferred, content.size());
    }

    {
        FILE* file = std::fopen(filename.c_str(), "rb");
        FileReader reader(file);
        std::vector<uint8_t> buffer(4);
        auto result = reader.read(buffer.data(), buffer.size());
        std::fclose(file);
        ASSERT_TRUE(result.ok());
        ASSERT_EQ(buffer, content);
    }

    std::remove(filename.c_str());  
}

TEST(IStreamOStreamTest, WriteThenReadBack) {
    std::stringstream stream;

    std::vector<uint8_t> originalData = { 'G', 'P', 'T', '-', '4' };

    gocxx::io::OStreamWriter writer(stream);
    auto writeResult = writer.write(originalData.data(), originalData.size());

    ASSERT_TRUE(writeResult.ok());
    ASSERT_EQ(writeResult.bytesTransferred, originalData.size());

    stream.seekg(0);

    std::vector<uint8_t> readData(originalData.size());
    gocxx::io::IStreamReader reader(stream);
    auto readResult = reader.read(readData.data(), readData.size());

    ASSERT_TRUE(readResult.ok());
    ASSERT_EQ(readResult.bytesTransferred, originalData.size());
    ASSERT_EQ(readData, originalData);
}


int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}