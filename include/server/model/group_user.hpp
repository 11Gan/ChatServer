#ifndef __GROUP_USER_H__
#define __GROUP_USER_H__

#include "user.hpp"
#include <string>

class GroupUser : public User
{
public:
    GroupUser() = default;
    void setRole(const std::string& role) { _role = role; }
    std::string getRole() { return _role; }

private:
    std::string _role;
};


#endif // __GROUP_USER_H__