let _ = require('lodash')
let fs = require('fs'),
    util = require('util'),
    stream = require('stream'),
    split = require('split2')
const { performance } = require('perf_hooks');
let args = process.argv.slice(2);
let file_path = args[0]
let begin_time = performance.now()
let end_time = ""
let decompose_word = (characters) => {
    return _.reduce(characters, (res, val, key) => {
        res[val] = (res[val] || (res[val] = 0)) + 1
        return res
    }, {})
}

let list_of_word_index = 1
let process_dic = (words_path, seeds) => {
    let seeds_decomposition = seeds.map((word, index) => {
        return {
            original: word,
            decomposition: decompose_word(word),
            position: index,
            creation_indexes: []
        }
    })
    let s = fs.createReadStream(words_path, { encoding: 'utf8' })
        .pipe(split()).on('data', (line) => {
            let curr_word_map = decompose_word(line)
            seeds_decomposition.forEach(element => {
                const clone = _.cloneDeep(element)
                let d = _.mergeWith(clone.decomposition, curr_word_map, (a, b) => (a || 0) - b)
                if (_.every(_.values(d), (v) => v >= 0))
                    element.creation_indexes.push(list_of_word_index)
            });
            ++list_of_word_index
        }).on('end', () => {
            end_time = performance.now()
            process.stdout.write("Took " + (end_time - begin_time) + "milliseconds\n")
            seeds_decomposition.forEach(seed => { process.stdout.write(seed.creation_indexes.join(' ') + "\n") });
        })
}

let parseInput = () => {
    let seeds = []
    fs.createReadStream(file_path, { encoding: 'utf8' }).pipe(split())
        .on('data', (seed) => {seeds.push(seed)})
        .on('end', () => { process_dic("liste_des_mots.txt", seeds)})
        .on('err', (v) => { console.log(err, v) })
}

parseInput()