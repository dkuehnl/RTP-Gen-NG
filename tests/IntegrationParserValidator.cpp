//
// Created by dkueh on 28.05.2026.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "YamlParser.h"
#include "StreamOptions.h"
#include "StreamOptionValidator.h"

class IntegrationParserValUNIXTest : public ::testing::Test {
protected:
    std::string complete_unix_config = std::string(TEST_DATA_DIR) + "complete_unix_config.yaml";
    std::string minimal_unix_config = std::string(TEST_DATA_DIR) + "minimal_unix_config.yaml";
    std::string invalid_config = std::string(TEST_DATA_DIR) + "invalid_config.yaml";
};

TEST_F(IntegrationParserValUNIXTest, ParseMinimalConfigWithoutError) {
    EXPECT_NO_THROW({
        auto parsed_config = yaml::parse(minimal_unix_config);
        const auto validation_result = sov::check_configuration(parsed_config);

        EXPECT_TRUE(validation_result.ok);
        EXPECT_TRUE(validation_result.errors.empty());
    });
}

TEST_F(IntegrationParserValUNIXTest, ParseMaxConfigWithoutError) {
    EXPECT_NO_THROW({
        auto parsed_config = yaml::parse(complete_unix_config);
        const auto validation_result = sov::check_configuration(parsed_config);

        EXPECT_TRUE(validation_result.ok);
        EXPECT_TRUE(validation_result.errors.empty());
    });
}

TEST_F(IntegrationParserValUNIXTest, NoExceptionButValidationError) {
    EXPECT_NO_THROW({
        auto parsed_config = yaml::parse(invalid_config);
        const auto validation_result = sov::check_configuration(parsed_config);

        EXPECT_FALSE(validation_result.ok);
        EXPECT_THAT(validation_result.errors, ::testing::Contains("Invalid source-port set"));
    });
}

class IntegrationParserValAMITest : public ::testing::Test {
protected:
    std::string complete_ami_config = std::string(TEST_DATA_DIR) + "complete_ami_config.yaml";
    std::string minimal_ami_config = std::string(TEST_DATA_DIR) + "minimal_ami_config.yaml";
    std::string invalid_config = std::string(TEST_DATA_DIR) + "invalid_config.yaml";
};

TEST_F(IntegrationParserValAMITest, ParseMinimalConfigWithoutError) {
    EXPECT_NO_THROW({
        auto parsed_config = yaml::parse(minimal_ami_config);
        const auto validation_result = sov::check_configuration(parsed_config);

        EXPECT_TRUE(validation_result.ok);
        EXPECT_TRUE(validation_result.errors.empty());
    });
}

TEST_F(IntegrationParserValAMITest, ParseMaxConfigWithoutError) {
    EXPECT_NO_THROW({
        auto parsed_config = yaml::parse(complete_ami_config);
        const auto validation_result = sov::check_configuration(parsed_config);

        EXPECT_TRUE(validation_result.ok);
        EXPECT_TRUE(validation_result.errors.empty());
    });
}

TEST_F(IntegrationParserValAMITest, NoExceptionButValidationError) {
    EXPECT_NO_THROW({
        auto parsed_config = yaml::parse(invalid_config);
        const auto validation_result = sov::check_configuration(parsed_config);

        EXPECT_FALSE(validation_result.ok);
        EXPECT_THAT(validation_result.errors, ::testing::Contains("Invalid source-port set"));
    });
}