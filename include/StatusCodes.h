/**
 *  @file   PandoraPFANew/include/StatusCodes.h
 * 
 *  @brief  Header file defining status codes and relevant preprocessor macros
 * 
 *  $Log: $
 */
#ifndef STATUS_CODES_H
#define STATUS_CODES_H 1

#include <exception>
#include <string>

/**
 *  @brief  The StatusCode enum
 */
enum StatusCode
{
    STATUS_CODE_SUCCESS,
    STATUS_CODE_FAILURE,
    STATUS_CODE_NOT_FOUND,
    STATUS_CODE_NOT_INITIALIZED,
    STATUS_CODE_ALREADY_INITIALIZED,
    STATUS_CODE_OUT_OF_RANGE,
    STATUS_CODE_NOT_ALLOWED
};

#define PANDORA_RETURN_RESULT_IF(StatusCode1, Operator, Command)                                \
    {                                                                                           \
        StatusCode statusCode = Command;                                                        \
        if (statusCode Operator StatusCode1)                                                    \
        {                                                                                       \
            std::cout << #Command << " return " << StatusCodeToString(statusCode) << std::endl; \
            return statusCode;                                                                  \
        }                                                                                       \
    }

#define PANDORA_RETURN_RESULT_IF_AND_IF(StatusCode1, StatusCode2, Operator, Command)            \
    {                                                                                           \
        StatusCode statusCode = Command;                                                        \
        if ((statusCode Operator StatusCode1) && (statusCode Operator StatusCode2))             \
        {                                                                                       \
            std::cout << #Command << " return " << StatusCodeToString(statusCode) << std::endl; \
            return statusCode;                                                                  \
        }                                                                                       \
    }

#define PANDORA_THROW_RESULT_IF(StatusCode1, Operator, Command)                                 \
    {                                                                                           \
        StatusCode statusCode = Command;                                                        \
        if (statusCode Operator StatusCode1)                                                    \
        {                                                                                       \
            std::cout << #Command << " throw " << StatusCodeToString(statusCode) << std::endl;  \
            throw StatusCodeException(statusCode);                                              \
        }                                                                                       \
    }

#define PANDORA_THROW_RESULT_IF_AND_IF(StatusCode1, StatusCode2, Operator, Command)             \
    {                                                                                           \
        StatusCode statusCode = Command;                                                        \
        if ((statusCode Operator StatusCode1) && (statusCode Operator StatusCode2))             \
        {                                                                                       \
            std::cout << #Command << " throw " << StatusCodeToString(statusCode) << std::endl;  \
            throw StatusCodeException(statusCode);                                              \
        }                                                                                       \
    }

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  StatusCodeException class
 */
class StatusCodeException : public std::exception
{
  public:
    /**
     *  @brief  Constructor
     * 
     *  @param  statusCode the status code
     */
    StatusCodeException(const StatusCode statusCode);

    /**
     *  @brief  Get status code
     * 
     *  @return the status code
     */
    StatusCode GetStatusCode() const;

    /**
     *  @brief  Get status code as a string
     * 
     *  @return The status code string
     */
    std::string ToString() const;

  private:
    StatusCode    m_statusCode;    ///< The status code
};

/**
 *  @brief  Get status code as a string
 * 
 *  @return The status code string
 */
std::string StatusCodeToString(const StatusCode statusCode);

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCodeException::StatusCodeException(const StatusCode statusCode) :
    m_statusCode(statusCode)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode StatusCodeException::GetStatusCode() const
{
    return m_statusCode;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline std::string StatusCodeException::ToString() const
{
    return StatusCodeToString(m_statusCode);
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline std::string StatusCodeToString(const StatusCode statusCode)
{
    switch (statusCode)
    {
    case STATUS_CODE_SUCCESS:
        return "STATUS_CODE_SUCCESS";

    case STATUS_CODE_FAILURE:
        return "STATUS_CODE_FAILURE";
        
    case STATUS_CODE_NOT_FOUND:
        return "STATUS_CODE_NOT_FOUND";
        
    case STATUS_CODE_NOT_INITIALIZED:
        return "STATUS_CODE_NOT_INITIALIZED";
        
    case STATUS_CODE_ALREADY_INITIALIZED:
        return "STATUS_CODE_ALREADY_INITIALIZED";
        
    case STATUS_CODE_OUT_OF_RANGE:
        return "STATUS_CODE_OUT_OF_RANGE";
        
    case STATUS_CODE_NOT_ALLOWED:
        return "STATUS_CODE_NOT_ALLOWED";
        
    default:
        return "STATUS_CODE_UNRECOGNIZED";
    }
}

#endif // #ifndef STATUS_CODES_H
