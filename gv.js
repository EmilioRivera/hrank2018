let _ = require('lodash')
let split = require('split2')
let args = process.argv.slice(2);
let min_heap = require('min-heap')
let filePath = args[0]

let decompose_word = (characters) => {
    let m = _.reduce(characters, (res, val, key) => {
        res[val] = (res[val] || (res[val] = 0)) + 1
        return res
    }, {})
    return m
}
let fs = require('fs'), util = require('util'), stream = require('stream')
let bib_index = {}
let list_of_word_index = 1
let process_dic = (words_path, seeds) => {
    let seeds_decomposition = seeds.map((word,index) => {
        return {
            original: word,
            decomp: decompose_word(word),
            position: index,
            creation_indexes: []
        }
    })
    let s = fs.createReadStream(words_path, { encoding: 'utf8' })
        .pipe(split())
        .on('data', (line) => {
            // Current line contain0s a word from dictionnary
            let curr_word_map = decompose_word(line)
            for (let i = 0; i < seeds_decomposition.length; i++) {
                // Since mergeWith mutates the object we need to save it and restore it later
                const elem = _.cloneDeep(seeds_decomposition[i])
                let d = _.mergeWith(elem.decomp , curr_word_map, (a,b) => (a || 0) - b)
                if (_.every(_.values(d), (v) => v >= 0)){
                    // console.log(`${line} can be constructed from ${elem.original}`)
                    seeds_decomposition[i].creation_indexes.push(list_of_word_index)
                }
            }
            ++list_of_word_index
            process.stdout.write("Processed " + list_of_word_index + "\r")
        }).on('end', async () => {
            process.stdout.write("\nDone\n")
            // console.log(bib_index)
            // let words = await parseInput()
            // We have an index containing the first letters
            seeds_decomposition.forEach(element => {
                process.stdout.write(element.creation_indexes.join('_')+"\n")
            });
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
            process_dic("liste_des_mots.txt", seeds)
        })
        .on('err', (v) => {
            console.log(err,v)
        })
}

parseInput()