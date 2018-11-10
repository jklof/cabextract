#include "gtest/gtest.h"
#include "cabextract.hh"

TEST(Cab, JustConstructAndDestruct)
{
    CabExtract cab;
}
TEST(Cab, MountCabNotFound)
{
    CabExtract cab;
    ASSERT_EQ(-1, cab.MountCab("/tmp/foooo"));
}

TEST(Cab, MountCab)
{
    CabExtract cab;
    ASSERT_EQ(0, cab.MountCab("./testdata/test.cab"));
}

TEST(Cab, ListFiles)
{
    CabExtract cab;
    ASSERT_EQ(0, cab.MountCab("./testdata/test.cab"));

    FindCabData data;
    ASSERT_EQ(1, cab.FindNextFileInCab(&data));
    ASSERT_EQ(data.name, "beach-clouds-daytime-994605.jpg");

    ASSERT_EQ(1, cab.FindNextFileInCab(&data));
    ASSERT_EQ(data.name, "hello.txt");

    ASSERT_EQ(1, cab.FindNextFileInCab(&data));
    ASSERT_EQ(data.name, "this_is_a_test.txt");

    ASSERT_EQ(0, cab.FindNextFileInCab(&data));
}

TEST(Cab, ListFilesTwice)
{
    CabExtract cab;
    ASSERT_EQ(0, cab.MountCab("./testdata/test.cab"));

    // read all files to list
    std::vector<FindCabData> list_one;
    FindCabData data;
    while (cab.FindNextFileInCab(&data)) {
        list_one.push_back(data);
        std::cout << data.name << " " << data.length << "\n";
    }

    // do it again, resetting with FindFirstFileInCab()
    std::vector<FindCabData> list_two;
    cab.FindFirstFileInCab(&data);
    list_two.push_back(data);
    while (cab.FindNextFileInCab(&data)) {
        list_two.push_back(data);
    }

    // two lists should exactly match
    ASSERT_EQ(list_one, list_two);
}

TEST(Cab, OpenFile)
{
    CabExtract cab;
    ASSERT_EQ(0, cab.MountCab("./testdata/test.cab"));

    HANDLE h = -1;
    ASSERT_EQ(0, cab.OpenFileInCab("hello.txt", &h));
    ASSERT_NE(-1, h);
}


TEST(Cab, OpenFileDoestExist)
{
    CabExtract cab;
    ASSERT_EQ(0, cab.MountCab("./testdata/test.cab"));
    HANDLE h = -1;
    ASSERT_EQ(-1, cab.OpenFileInCab("not_hello.txt", &h));
}


TEST(Cab, ReadFile)
{
    CabExtract cab;
    ASSERT_EQ(0, cab.MountCab("./testdata/test.cab"));

    HANDLE h = -1;
    ASSERT_EQ(0, cab.OpenFileInCab("hello.txt", &h));

    // expected hello file contents
    std::vector<uint8_t> expected  { 'H','e','l','l','o', ' ', 'w', 'o', 'r', 'l', 'd', '!' };

    DWORD bytes = expected.size();
    std::vector<uint8_t> readbuf(bytes);

    ASSERT_EQ(0, cab.ReadFromFileInCab(h, 0, readbuf.data(), &bytes));
    ASSERT_EQ((DWORD)expected.size(), bytes);
    ASSERT_EQ(readbuf, expected);

    cab.CloseFileInCab(h);

}

