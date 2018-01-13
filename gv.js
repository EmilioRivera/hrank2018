let fs = require('fs')
let args = process.argv.slice(2);

let file_path = args[0]
let decompose_word = (characters) => {
    return characters.split('').reduce((res, val, key) => {
        res[val] = (res[val] || (res[val] = 0)) + 1
        return res
    }, {})
}

let isWordCreatableFrom = (seed_decomposition, actual_word) => {
    for (const key in actual_word) {
        if (actual_word.hasOwnProperty(key)) {
            // Check if seed at least has the value
            if (!seed_decomposition.hasOwnProperty(key))
                return false
            if (seed_decomposition[key] < actual_word[key])
                return false
        }
    }
    return true
}

let process_dictionary = (words_path, seeds) => {
    let seeds_decomposition = seeds.map((word, index) => {
        return {
            decomposition: decompose_word(word),
            position: index,
            creation_indexes: []
        }
    })
    let lines = fs.readFileSync(words_path, { encoding: 'utf8' }).split('\r\n')
    lines.forEach((word_from_dictionnary, i) => {
        let word_decomposition = decompose_word(word_from_dictionnary)
        seeds_decomposition.forEach(element => {
            if (isWordCreatableFrom(element.decomposition, word_decomposition))
                element.creation_indexes.push(i+1) // Index is zero-based whereas we want 1-based index
        });
    })
    let writer = fs.createWriteStream('out.txt',{autoClose: true})
    for (let i = 0; i < seeds_decomposition.length; i++) {
        const l = (seeds_decomposition[i] || []).length;
        for (let j = 0; j < l; j++) {
            const an = seeds_decomposition[i][j];
            writer.write(an + " ")
        }
        writer.write("\n")
    }
}

let seeds = fs.readFileSync(file_path, { encoding: 'utf8' }).split('\r\n')
process_dictionary("liste_des_mots.txt", seeds)
