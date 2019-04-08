'use strict';

// let text = `
// 1
// 22 20 4
// 191 438849
// 194 69510
// 191 7274
// 194 692463
// `;

let text = `
1
22 20 3
191 438849
194 69510
192 7274
`;

function read_input( text ) {
	let ret = [];
	let arr = text.split( '\n' );
	if ( arr[ 0 ] === '' ) {
		arr.shift();
	}

	function _get_test_case( lines ) {
		let head = lines[ 0 ].split( ' ' );
		let num_minions = head[ 2 ];
		let obj = {
			P: parseFloat( head[ 0 ] ),
			Q: parseFloat( head[ 1 ] ),
			minions: []
		};

		for ( let i = 0; i < num_minions; i++ ) {
			let line = lines[ i + 1 ].split( ' ' );
			obj.minions.push( {
				H: parseFloat( line[ 0 ] ),
				G: parseFloat( line[ 1 ] )
			} );
		}
		lines.splice( 0, num_minions + 1 );
		return obj;
	}

	let num_cases = parseInt( arr[ 0 ] );
	arr.shift();
	for ( let i = 0; i < num_cases; i++ ) {
		ret.push( _get_test_case( arr ) );
	}
	return ret;
}

function cs( test ){
	let max_gold = 0;
	let results = [];
	let outcomes = 0;

	function cs2( { minions, P, Q, gold = 0, actions = [] } ) {
		let is_not_done = minions.reduce( ( p, c ) => {
			return p || c > 0;
		}, false );

		if ( !is_not_done ) {
			outcomes++;
			if( outcomes % 10000 === 0 ){
				console.log( outcomes.toLocaleString() );
			}
			if ( gold > max_gold ) {
				max_gold = gold;
				results.push( {
					gold: gold,
					actions: actions
				} );
			}
			return gold;
		}

		let gold_values = [];

		for( let i = 0; i < minions.length + 1; i++ ){
			let local_gold = gold;
			let local_actions = actions.slice();
			let local_minions = minions.slice();

			if( local_minions[ i ] <= 0 ){
				continue;
			}

			if( i === minions.length ) {
				local_actions.push( 'Diana refrains.' );
			} else {
				local_actions.push( 'Diana attacks minion' + i + ' (' + local_minions[ i ] + 'hp) with ' + P + ' damage.' );
				local_minions[ i ] -= P;
				if( local_minions[ i ] <= 0 ){
					local_actions.push( 'Diana kills minion' + i + ' (+' + test.minions[ i ].G + 'gp).' );
					local_gold += test.minions[ i ].G;
				}
			}

			for( let j = 0; j < minions.length; j++ ){
				if( local_minions[ j ] > 0 ) {
					local_actions.push( 'Tower attacks minion' + j + ' (' + local_minions[ j ] + 'hp) with ' + Q + ' damage.' );
					local_minions[ j ] -= Q;
					if( local_minions[ j ] <= 0 ) {
						local_actions.push( 'Tower kills minion' + j + '.' );
					}
					break;
				}
			}
			gold_values.push( cs2( { minions: local_minions, P, Q, gold: local_gold, actions: local_actions } ) );
		}

		return gold_values.reduce( ( p, c ) => {
			return c > p ? c : p;
		}, 0 );
	}

	let ret = {
		gold: cs2( { minions: test.minions.map( ( m ) => m.H ), P: test.P, Q: test.Q } ),
		results: results
	};

	console.log( 'Done simulating', outcomes.toLocaleString(), 'outcomes' );

	return ret;
}

let obj = read_input( text );
// console.log( JSON.stringify( obj ) );
for ( let test of obj ) {
	let o = cs( test );
	console.log( 'GOLD: ' + o.gold );
	o.results.filter( ( a ) => {
		return a.gold === o.gold;
	} ).forEach( ( obj, i, arr ) => {
		if( i === 0 ) {
			console.log( 'RESULT #' + ( i + 1 ), 'of', arr.length );
			obj.actions.forEach( ( a, i ) => {
				console.log( ( i + 1 ) + '.)', a );
			} );
		}
	} );
}
