#include "filter/xorfilter_modi.h"
#include <string>
#include <vector>

namespace xorfilter {
void applyBlock(uint64_t *tmp, int b, int len, t2val_t *t2vals) {
  for (int i = 0; i < len; i += 2) {
    uint64_t x = tmp[(b << blockShift) + i];
    int index = (int)tmp[(b << blockShift) + i + 1];
    t2vals[index].t2count++;
    t2vals[index].t2 ^= x;
  }
}

int applyBlock2(uint64_t *tmp, int b, int len, t2val_t *t2vals, int *alone,
                int alonePos) {
  for (int i = 0; i < len; i += 2) {
    uint64_t hash = tmp[(b << blockShift) + i];
    int index = (int)tmp[(b << blockShift) + i + 1];
    int oldCount = t2vals[index].t2count;
    if (oldCount >= 1) {
      int newCount = oldCount - 1;
      t2vals[index].t2count = newCount;
      if (newCount == 1) {
        alone[alonePos++] = index;
      }
      t2vals[index].t2 ^= hash;
    }
  }
  return alonePos;
}

XorFilter::XorFilter(const std::string &filename) {
  std::ifstream file(filename);
  if (!file.good()) {
    std::cout << "Deserialize false, File doesn't exsit." << std::endl;
  }
  if (file.is_open()) {
    std::string sizeStr;
    std::string arrayLengthStr;
    std::string blockLengthStr;
    std::string seedStr;
    std::string hashIndexStr;
    std::string fingerprintLenStr;

    if (!std::getline(file, sizeStr)) {
      std::cerr << "Error reading size." << std::endl;
    }
    this->size = static_cast<size_t>(std::stoull(sizeStr));

    if (!std::getline(file, arrayLengthStr)) {
      std::cerr << "Error reading arrayLength." << std::endl;
    }
    this->arrayLength = static_cast<size_t>(std::stoull(arrayLengthStr));

    if (!std::getline(file, blockLengthStr)) {
      std::cerr << "Error reading blockLength." << std::endl;
    }
    this->blockLength = static_cast<size_t>(std::stoull(blockLengthStr));

    if (!std::getline(file, seedStr)) {
      std::cerr << "Error reading seed." << std::endl;
    }
    this->hasher = SpookyHash(std::stoull(seedStr));

    if (!std::getline(file, hashIndexStr)) {
      std::cerr << "Error reading hashIndex." << std::endl;
    }
    this->hashIndex = static_cast<size_t>(std::stoull(hashIndexStr));

    if (!std::getline(file, fingerprintLenStr)) {
      std::cerr << "Error reading hashIndex." << std::endl;
    }
    this->fingerprintLen = static_cast<size_t>(std::stoull(fingerprintLenStr));

    this->fingerprints = new std::string[this->arrayLength];
    for (int i = 0; i < this->arrayLength; i++) {
      char buffer[fingerprintLen];
      file.read(buffer, fingerprintLen);
      fingerprints[i] = std::string(buffer, fingerprintLen);
    }
    file.close();
  } else {
    std::cerr << "Unable to open file for deserialization." << std::endl;
  }
}

void XorFilter::Serialize(const std::string &filename) {
  std::ofstream file(filename);
  if (file.is_open()) {
    file << size << "\n";
    file << arrayLength << "\n";
    file << blockLength << "\n";
    file << hasher.GetSeed() << "\n";
    file << hashIndex << "\n";
    file << fingerprintLen << "\n";

    for (int i = 0; i < arrayLength; i++) {
      file << fingerprints[i];
    }

    file.close();
  } else {
    std::cerr << "Unable to open file for serialization." << std::endl;
  }
}
Status XorFilter::AddAll(std::map<std::string, std::string> &kvPairs) {

  fingerprintLen = kvPairs.begin()->second.size();
  for (int i = 0; i < this->arrayLength; i++) {
    fingerprints[i] = GenRandomString(fingerprintLen);
  }
  int m = arrayLength;
  uint64_t *reverseOrder = new uint64_t[size];
  uint8_t *reverseH = new uint8_t[size];
  size_t reverseOrderPos;
  hashIndex = 0;
  t2val_t *t2vals = new t2val_t[m];
  std::map<uint64_t, std::string> newPairs;

  while (true) {
    memset(t2vals, 0, sizeof(t2val_t[m]));
    int blocks = 1 + ((3 * blockLength) >> blockShift);
    uint64_t *tmp = new uint64_t[blocks << blockShift];
    int *tmpc = new int[blocks]();
    for (auto it = kvPairs.begin(); it != kvPairs.end(); it++) {
      std::string k = it->first; // string can be converted into?
      uint64_t hash = hasher.Hash64(&k[0], k.size());
      newPairs[hash] = kvPairs[k];
      for (int hi = 0; hi < 3; hi++) {
        int index = getHashFromHash(hash, hi, blockLength);
        int b = index >> blockShift;
        int i2 = tmpc[b];
        tmp[(b << blockShift) + i2] = hash;
        tmp[(b << blockShift) + i2 + 1] = index;
        tmpc[b] += 2;
        if (i2 + 2 == (1 << blockShift)) {
          applyBlock(tmp, b, i2 + 2, t2vals);
          tmpc[b] = 0;
        }
      }
    }
    for (int b = 0; b < blocks; b++) {
      applyBlock(tmp, b, tmpc[b], t2vals);
    }
    delete[] tmp;
    delete[] tmpc;
    reverseOrderPos = 0;

    // 初次分配完毕，下面进行map操作的后半段，寻找count为1的
    int *alone = new int[arrayLength];
    int alonePos = 0;
    for (size_t i = 0; i < arrayLength; i++) {
      if (t2vals[i].t2count == 1) {
        alone[alonePos++] = i;
      }
    }
    tmp = new uint64_t[blocks << blockShift];
    tmpc = new int[blocks]();
    reverseOrderPos = 0;
    int bestBlock = -1;
    while (reverseOrderPos < size) {
      if (alonePos == 0) {
        // we need to apply blocks until we have an entry that is alone
        // (that is, until alonePos > 0)
        // so, find a large block (the larger the better)
        // but don't need to search very long
        // start searching where we stopped the last time
        // (to make it more even)
        for (int i = 0, b = bestBlock + 1, best = -1; i < blocks; i++) {
          if (b >= blocks) {
            b = 0;
          }
          if (tmpc[b] > best) {
            best = tmpc[b];
            bestBlock = b;
            if (best > (1 << (blockShift - 1))) {
              // sufficiently large: stop
              break;
            }
          }
        }
        if (tmpc[bestBlock] > 0) {
          alonePos = applyBlock2(tmp, bestBlock, tmpc[bestBlock], t2vals, alone,
                                 alonePos);
          tmpc[bestBlock] = 0;
        }
        // applying a block may not actually result in a new entry that is alone
        if (alonePos == 0) {
          for (int b = 0; b < blocks && alonePos == 0; b++) {
            if (tmpc[b] > 0) {
              alonePos = applyBlock2(tmp, b, tmpc[b], t2vals, alone, alonePos);
              tmpc[b] = 0;
            }
          }
        }
      }
      if (alonePos == 0) {
        break;
      }
      int i = alone[--alonePos];
      int b = i >> blockShift;
      if (tmpc[b] > 0) {
        alonePos = applyBlock2(tmp, b, tmpc[b], t2vals, alone, alonePos);
        tmpc[b] = 0;
      }
      uint8_t found = -1;
      if (t2vals[i].t2count == 0) {
        continue;
      }
      long hash = t2vals[i].t2;
      for (int hi = 0; hi < 3; hi++) {
        int h = getHashFromHash(hash, hi, blockLength);
        if (h == i) {
          found = (uint8_t)hi;
          t2vals[i].t2count = 0;
        } else {
          int b = h >> blockShift;
          int i2 = tmpc[b];
          tmp[(b << blockShift) + i2] = hash;
          tmp[(b << blockShift) + i2 + 1] = h;
          tmpc[b] += 2;
          if (tmpc[b] >= 1 << blockShift) {
            alonePos = applyBlock2(tmp, b, tmpc[b], t2vals, alone, alonePos);
            tmpc[b] = 0;
          }
        }
      }
      reverseOrder[reverseOrderPos] = hash;
      reverseH[reverseOrderPos] = found;
      reverseOrderPos++;
    }
    delete[] tmp;
    delete[] tmpc;
    delete[] alone;

    if (reverseOrderPos == kvPairs.size()) {
      break;
    }

    hashIndex++;

    // use a new random numbers
    hasher = SpookyHash();
  }

  for (int i = reverseOrderPos - 1; i >= 0; i--) {
    // the hash of the key we insert next
    uint64_t hash = reverseOrder[i];
    int found = reverseH[i];
    // which entry in the table we can change
    int change = -1;
    // we set table[change] to the fingerprint of the key,
    // unless the other two entries are already occupied
    std::string xor2 = newPairs[hash]; // change
    for (int hi = 0; hi < 3; hi++) {
      size_t h = getHashFromHash(hash, hi, blockLength);
      if (found == hi) {
        change = h;
      } else {
        // this is different from BDZ: using xor to calculate the
        // fingerprint
        xor2 = FingerprintXor(xor2, fingerprints[h]);
      }
    }
    fingerprints[change] = xor2;
  }
  delete[] t2vals;
  delete[] reverseOrder;
  delete[] reverseH;

  return Ok;
}

std::string XorFilter::Retrieval(const std::string &key) {
  uint64_t hash = hasher.Hash64(&key[0], key.size());
  // std::cout << "Hash: " << hash << endl;
  uint32_t r0 = (uint32_t)hash;
  uint32_t r1 = (uint32_t)rotl64(hash, 21);
  uint32_t r2 = (uint32_t)rotl64(hash, 42);
  uint32_t h0 = reduce(r0, blockLength);
  uint32_t h1 = reduce(r1, blockLength) + blockLength;
  uint32_t h2 = reduce(r2, blockLength) + 2 * blockLength;
  // std::cout << "h0, h1, h2: " << h0 << ", " << h1 << ", " << h2 << endl;
  std::string f = FingerprintXor(fingerprints[h0], fingerprints[h1]);
  f = FingerprintXor(f, fingerprints[h2]);
  // delete extra '\0' in the tail wrong!!!
  /*
  size_t pos = f.find_last_not_of('\0');
  if (pos != std::string::npos) {
    f.erase(pos + 1);
  }
  */
  return f;
}

Status XorFilter::Contain(const std::string &key,
                          std::map<std::string, std::string> &kvPairs) {
  uint64_t hash = hasher.Hash64(&key[0], key.size());
  std::string f = kvPairs[key];
  uint32_t r0 = (uint32_t)hash;
  uint32_t r1 = (uint32_t)rotl64(hash, 21);
  uint32_t r2 = (uint32_t)rotl64(hash, 42);
  uint32_t h0 = reduce(r0, blockLength);
  uint32_t h1 = reduce(r1, blockLength) + blockLength;
  uint32_t h2 = reduce(r2, blockLength) + 2 * blockLength;
  f = FingerprintXor(f, fingerprints[h0]);
  f = FingerprintXor(f, fingerprints[h1]);
  f = FingerprintXor(f, fingerprints[h2]);
  Status flag = Ok;
  for (auto c : f) {
    if (c != '\0') {
      flag = NotFound;
      break;
    }
  }
  return flag;
}

std::string XorFilter::Info() const {
  std::stringstream ss;
  ss << "XorFilter Status:\n"
     << "\t\tKeys stored: " << Size() << "\n";
  return ss.str();
}
} // namespace xorfilter
