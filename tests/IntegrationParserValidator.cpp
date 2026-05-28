//
// Created by dkueh on 28.05.2026.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "YamlParser.h"
#include "StreamOptions.h"
#include "StreamOptionValidator.h"

class IntegrationParserValTest : public ::testing::Test {
protected:
    std::string complete_config = std::string(TEST_DATA_DIR) + "complete_config.yaml";
    std::string minimal_config = std::string(TEST_DATA_DIR) + "minimal_config.yaml";
    std::string invalid_config = std::string(TEST_DATA_DIR) + "invalid_config.yaml";
};

TEST_F(IntegrationParserValTest, ParseMinimalConfigWithoutError) {
    EXPECT_NO_THROW({
        auto parsed_config = yaml::parse(minimal_config);
        const auto validation_result = sov::check_configuration(parsed_config);

        EXPECT_TRUE(validation_result.ok);
        EXPECT_TRUE(validation_result.errors.empty());
    });
}

TEST_F(IntegrationParserValTest, ParseMaxConfigWithoutError) {
    EXPECT_NO_THROW({
        auto parsed_config = yaml::parse(complete_config);
        const auto validation_result = sov::check_configuration(parsed_config);

        EXPECT_TRUE(validation_result.ok);
        EXPECT_TRUE(validation_result.errors.empty());
    });
}

TEST_F(IntegrationParserValTest, NoExceptionButValidationError) {
    EXPECT_NO_THROW({
        auto parsed_config = yaml::parse(invalid_config);
        const auto validation_result = sov::check_configuration(parsed_config);

        EXPECT_FALSE(validation_result.ok);
        EXPECT_THAT(validation_result.errors, ::testing::Contains("Invalid source-port set"));
    });
}