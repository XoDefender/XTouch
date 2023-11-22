#include <iconv.h>

class iconv_wrapper
{
private:
    iconv_t cd;

public:
    iconv_wrapper(std::string_view from, std::string_view to = "UTF-8")
        : cd{iconv_open(to.data(), from.data())}
    {
        if (cd == reinterpret_cast<iconv_t>(-1))
        {
            // Need better error handling
            std::cerr << "Unable to open converter from " << from << " to " << to
                      << ": " << std::strerror(errno) << '\n';
            std::exit(EXIT_FAILURE);
        }
    }
    ~iconv_wrapper() noexcept { iconv_close(cd); }
    std::string convert(std::string_view input)
    {
        // Work out the maximum output size (Assuming converting from a
        // single-byte encoding to UTF-8) and allocate a buffer and the
        // other args needed for iconv
        std::size_t insize = input.size();
        std::size_t outsize = insize * 4;
        std::size_t orig_outsize = outsize;
        auto outbuf = std::make_unique<char[]>(outsize);
        char *indata = const_cast<char *>(&input[0]);
        char *outdata = &outbuf[0];

        // Convert the input argument
        try
        {
            std::size_t ret = iconv(cd, &indata, &insize, &outdata, &outsize);
            return std::string(outbuf.get(), orig_outsize - outsize);
        }
        catch (const std::exception &e)
        {
            return 0;
        }
    };
};