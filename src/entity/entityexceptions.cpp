#include <framework/entity/entityexceptions.h>

namespace windgale
{
    InvalidIdException::InvalidIdException(const std::string& message) : std::runtime_error(message)
    {
    };

    InvalidAttributeException::InvalidAttributeException(const std::string& m, const AttributeHash h) : std::runtime_error(m), hash(h) 
    {
    };
    
    EntityException::EntityException(const std::string& message) : std::runtime_error(message)
    {
    };
}