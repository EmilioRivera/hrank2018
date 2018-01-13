let _ = require('lodash')
let args = process.argv.slice(2);
let word = args[0]

shiftRight = (s) => _.last(s) + _.dropRight(s).join('')
isPal = (s) => _.reverse(s.split('')).join('') == s
let currWord = word
let minRotForPal = -1
for (var i = 1; i < word.length; i++) {
    currWord = shiftRight(currWord)
    if (isPal(currWord) && currWord != word)
        minRotForPal = i
}
console.log(`Answer: ${minRotForPal}`)
return minRotForPal