#pragma once

#include <string>

enum class UserRole {
    Customer,
    StoreAdmin,
    Admin
};

struct User {
    int id{};
    std::string username;
    std::string email;
    std::string passwordHash;
    UserRole role{ UserRole::Customer };
};
