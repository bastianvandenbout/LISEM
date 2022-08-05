import { convertToNumber, roundToFloat16Bits } from "./converter.mjs";
import { NumberIsFinite } from "./primordials.mjs";

/**
 * returns the nearest half-precision float representation of a number
 *
 * @param {number} x
 * @returns {number}
 */
export function hfround(x) {
  const number = +x;

  // for optimization
  if (!NumberIsFinite(number) || number === 0) {
    return number;
  }

  const x16 = roundToFloat16Bits(number);
  return convertToNumber(x16);
}
