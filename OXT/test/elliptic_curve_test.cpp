
#include <cstring>
#include <gtest/gtest.h>
#include <iostream>

#include <sstream>
#include <string>

#include <gmp.h>
#include <pbc/pbc.h>
#include <pbc/pbc_field.h>

TEST(EllipticCurveTest, SimpleTest) {
  pairing_t pairing;
  char param[1024];
  FILE *file = fopen("/Volumes/solid/code/project/test/d159.param", "r");
  size_t count = fread(param, 1, 1024, file);
  fclose(file);
  if (!count)
    pbc_die("input error");
  pairing_init_set_buf(pairing, param, count);

  element_t g;
  element_init_G1(g, pairing);
  element_random(g);
  std::cout << "g: ";
  element_printf("%B\n", g);

  std::string xind = "12345";
  element_t e_xind;
  element_init_Zr(e_xind, pairing);
  element_random(e_xind);
  element_from_hash(e_xind, (void *)(&xind[0]), xind.size());
  std::cout << "xind: ";
  element_printf("%B\n", e_xind);
  std::cout << std::endl;

  std::string z = "123";
  element_t e_z;
  element_init_Zr(e_z, pairing);
  element_random(e_z);
  element_from_hash(e_z, (void *)&z[0], z.size());
  std::cout << "z: ";
  element_printf("%B\n", e_z);
  std::cout << std::endl;

  element_t z_inv;
  element_init_Zr(z_inv, pairing);
  element_random(z_inv);
  element_invert(z_inv, e_z);
  std::cout << "z_inv: ";
  element_printf("%B\n", z_inv);
  std::cout << std::endl;

  element_t y;
  element_init_Zr(y, pairing);
  element_mul(y, e_xind, z_inv);
  std::cout << "y: ";
  element_printf("%B\n", y);
  std::cout << std::endl;

  element_t e_xtrap;
  std::string xtrap = "456";
  element_init_Zr(e_xtrap, pairing);
  element_random(e_xtrap);
  element_from_hash(e_xtrap, (void *)&xtrap[0], xtrap.size());

  element_t v;
  element_init_Zr(v, pairing);
  element_random(v);
  element_mul(v, e_xtrap, e_z);

  element_t xtagTrue;
  element_t xtrap_xind;
  element_init_Zr(xtrap_xind, pairing);
  element_random(xtrap_xind);
  element_init_G1(xtagTrue, pairing);
  element_random(xtagTrue);
  element_mul(xtrap_xind, e_xtrap, e_xind);
  element_pow_zn(xtagTrue, g, xtrap_xind);

  element_t xtagVeri;
  element_t xtrap_z_xind;
  element_init_Zr(xtrap_z_xind, pairing);
  element_random(xtrap_z_xind);
  element_init_G1(xtagVeri, pairing);
  element_random(xtagVeri);
  element_mul(xtrap_z_xind, v, y);
  element_pow_zn(xtagVeri, g, xtrap_z_xind);

  element_t xtagFalse;
  element_init_G1(xtagFalse, pairing);
  element_random(xtagFalse);

  int res1 = element_cmp(xtagTrue, xtagVeri);
  int res2 = element_cmp(xtrap_xind, xtrap_z_xind);
  int res3 = element_cmp(xtagTrue, xtagFalse);
  int res4 = element_cmp(xtagVeri, xtagFalse);
  // int size = pairing_length_in_bytes_x_only_G1(pairing);
  char *s = new char[105];
  memset(s, 0, 105);
  element_snprint(s, 105, xtagTrue);
  std::string str(s);
  std::cout << str << std::endl;
  std::cout << str.size() << std::endl;
  element_t xtagRecover;
  element_init_G1(xtagRecover, pairing);
  int res6 = element_set_str(xtagRecover, str.c_str(), 10);
  std::cout << "res6: " << res6 << std::endl;
  std::cout << "xtagTrue: ";
  element_printf("%B\n", xtagTrue);
  std::cout << "xtagRecover: ";
  element_printf("%B\n", xtagRecover);
  int res5 = element_cmp(xtagTrue, xtagRecover);
  pairing_clear(pairing);
}