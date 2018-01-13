let _ = require('lodash')
let split = require('split2')
let args = process.argv.slice(2);
let min_heap = require('min-heap')
let filePath = args[0]

let createWord = (characters) => {
    let m = _.reduce(characters, (res, val, key) => {
        res[val] = (res[val] || (res[val] = 0)) + 1
        return res
    }, {})
    return m
}

let w = createWord("word")
console.log(w)

let fs = require('fs'), util = require('util'), stream = require('stream')

let list_of_word_index = 1
let bib_index = {}
let process_dic = (words_path, seeds) => {
    let seeds_decomposition = seeds.map((word,index) => {
        return {
            original: word,
            decomp: createWord(word),
            position: index
        }
    })
    let s = fs.createReadStream("liste_des_mots.txt", { encoding: 'utf8' })
        .pipe(split())
        .on('data', (line) => {
            /* 
            console.log(`[${++list_of_word_index}] - ${line}`)
            let fisrt_letter = line[0]
            if (!_.has(bib_index, fisrt_letter)) {
                bib_index[fisrt_letter] = {
                    first: list_of_word_index,
                    last: list_of_word_index,
                }
            }
            bib_index[fisrt_letter].last = list_of_word_index

            ++list_of_word_index */
            // Current line contain0s a word from dictionnary
            let curr_word_map = createWord(line)
            for (let i = 0; i < seeds_decomposition.length; i++) {
                const elem = seeds_decomposition[i]
                let d = _.mergeWith(elem.decomp , curr_word_map, (a,b) => (a || 0) - b)
                if (_.every(_.values(d), (v) => v > 0)){
                    // the current word can be constructed from "elem.decomp"
                    // TODO: Write the index "list_of_word_index" at line i
                    console.log(`${line} can be constructed from ${elem.original}`)
                }
            }
        }).on('end', async () => {
            // console.log(bib_index)
            // let words = await parseInput()
            // We have an index containing the first letters
            console.log('Done')
        })
}


let parseInput = () => {
    let number_of_seeds = 0
    let length_heap = new min_heap((left, right) => {
        return left.length - right.length
    })
    let seeds = []
    fs.createReadStream(filePath, { encoding: 'utf8' })
        .pipe(split())
        .on('data', (seed) => {
            length_heap.insert(seed)
            seeds.push(seed)
            number_of_seeds++
        })
        .on('end', () => {
            process_dic("", seeds)
        })
        .on('err', () => {
            console.log(err)
        })
}

parseInput()