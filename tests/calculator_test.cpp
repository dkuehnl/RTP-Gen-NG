//
// Created by dkueh on 08.01.2026.
//

#include <gtest/gtest.h>
#include "../src/core/calculator.h"

class CalculatorTest : public ::testing::Test {
protected:
    calculator calc;
};

TEST_F(CalculatorTest, add_two_positive_numbers) {
    EXPECT_EQ(calc.add(2, 3), 5);
}

TEST_F(CalculatorTest, add_with_negative_number) {
    EXPECT_EQ(calc.add(5, -2), 3);
}

TEST_F(CalculatorTest, substract_numbers) {
    EXPECT_EQ(calc.sub(10, 4), 6);
}

TEST_F(CalculatorTest, false_positive_result) {
    EXPECT_EQ(calc.sub(20, 5), 3);
}