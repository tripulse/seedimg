#ifndef SEEDIMGGIF_HPP
#define SEEDIMGGIF_HPP

#include <seedimg.hpp>

namespace seedimg::modules {
namespace gif {
/**
 * @brief Checks if a given file is a valid GIF 87/89 file.
 * @param filename OS-specific path to load file from.
 * @return true if matched signature, false if not.
 */
bool check(const std::string& filename);

/**
 * @brief Encode a given image in the GIF format.
 * @param filename the filename to put imagedata in.
 * @param input the image to encode.
 * @return true if successful, false if failed.
 */
bool to(const std::string &filename, const simg &input);

/**
 * @brief from Decode an image from a given filepath.
 * @param filename filepath to load it from.
 * @return an image if successful, nullptr if not.
 */
simg from(const std::string &filename);
}
}

#endif // SEEDIMGGIF_HPP
