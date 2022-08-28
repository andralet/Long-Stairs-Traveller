#include "stat.hpp"

int door_offset_by_pos(int pos, const MapSettings &data) {
	if (pos < data.width - 2) // top
		return pos + 1;
	else if (pos < data.width - 2 + data.height - 2) // right
		return ((pos - data.width + 2) + 2) * data.width - 1;
	else if (pos < 2 * (data.width - 2) + data.height - 2) // bottom
		return (data.width * data.height - (pos - data.width + 2 - data.height + 2) - 2);
	else if (pos < 2 * (data.width - 2 + data.height - 2)) // left
		return (data.height - (pos - 2 * data.width - data.height + 6) - 2) * data.width;
	else {
		printf("BAD DOOR OFFSET!\n");
		return -1;
	}
}

void make_map(FILE *out, MapSettings data) {
    // basic header
    fprintf(out, "{\"header\":{\"app\":\"RPG Map 2\",\"appVersion\":16.2,\"appAuthor\":\"Sebastien Benard\",\"fileType\":\"JSON Map\",\"fileVersion\":10,\"url\":\"https://deepnight.net/tools/rpg-map/\"},\n");
    fprintf(out, "\"version\":10,\"w\":%d,\"h\":%d,\n", data.width, data.height);
	fprintf(out, "\"skin\":{\n\t\"base\":{\"bg\":5715237,\"appBg\":1052441,\"shadows\":425148975,\"stairs\":-1998203707,\"walls\":%d,\"dirt\":%d,\"doors\":15233090,\"windows\":6809278,\"fences\":11588072,\"furn\":13403491,\"wallOutline\":null,\"colorless\":false},\n", data.color.walls, data.color.stones); // dirt - stones
    fprintf(out, "\t\"plant1\":%d,\"plant2\":%d,\"water\":6657005,\"specialWater1\":16733696,\"specialWater2\":9478433,\"teint\":null,\n", data.color.plant1, data.color.plant2);
    fprintf(out, "\t\"grid\":{\"id\":3,\"p\":[]},\"gridColor\":16777215,\"gridIntensity\":0.2,\"gridDepth\":{\"id\":0,\"p\":[]},\n");
	fprintf(out, "\t\"fogColor\":2433347,\"fogIntensity\":0,\"seed\":848627,\"printerFriendly\":false,\"bubbleSkin\":{\"id\":0,\"p\":[]}\n},\n");
    fprintf(out, "\"furnDepth\":{\"id\":0,\"p\":[]},\"snapFurnToWalls\":true,\"legend\":{\"x\":0,\"y\":0},\n");

	// generating doors
	int door_num = data.door_num;
	int input_door = rand() % door_num;
	int max_pos = 2 * (data.width + data.height - 4);
	std::set<int> door_offsets = {}, door_pos = {};
	while (door_num > 0) {
		int pos = rand() % max_pos;
		if (door_pos.find(pos) != door_pos.end() || door_pos.find(pos - 1) != door_pos.end() || door_pos.find(pos + 1) != door_pos.end())
			continue;
		// else
		door_pos.insert(pos);
		door_num--;
		int offset = door_offset_by_pos(pos, data);
		if (offset >= 0) {
			door_offsets.insert(offset);
		} else {
			door_num++; // let's try again
		}
	}

    // building walls
    fprintf(out, "\"collisions\":[\"0:%d\"", data.wall);
    for (int i = 1; i < data.width; i++) {
		if (door_offsets.find(i) == door_offsets.end())
        	fprintf(out, ",\"%d:%d\"", i, data.wall);
    }
    for (int i = 1; i < data.height - 1; i++) {
		if (door_offsets.find(i * data.width) == door_offsets.end())
        	fprintf(out, ",\"%d:%d\"", i * data.width, data.wall);
		if (door_offsets.find((i + 1) * data.width - 1) == door_offsets.end())
        	fprintf(out, ",\"%d:%d\"", (i + 1) * data.width - 1, data.wall);
    }
    for (int i = 0; i < data.width; i++) {
		if (door_offsets.find((data.height - 1) * data.width + i) == door_offsets.end())
        	fprintf(out, ",\"%d:%d\"", (data.height - 1) * data.width + i, data.wall);
    }
    fprintf(out, "],\n");

    // filling ground tiles
	if (data.color.ground >= 0) {
    	fprintf(out, "\"grounds\":[{\"x\":1,\"y\":1,\"s\":{\"id\":%d,\"p\":[]},\"c\":%d}", data.ground, data.color.ground);
    	for (int i = 0; i < data.height; i++) {
        	for (int j = 0; j < data.width; j++) {
            	fprintf(out, ",{\"x\":%d,\"y\":%d,\"s\":{\"id\":%d,\"p\":[]},\"c\":%d}", j, i, data.ground, data.color.ground);
        	}
    	}
	} else {
		// use default color
    	fprintf(out, "\"grounds\":[{\"x\":1,\"y\":1,\"s\":{\"id\":%d,\"p\":[]},\"c\":null}", data.ground);
    	for (int i = 0; i < data.height; i++) {
        	for (int j = 0; j < data.width; j++) {
            	fprintf(out, ",{\"x\":%d,\"y\":%d,\"s\":{\"id\":%d,\"p\":[]},\"c\":null}", j, i, data.ground);
        	}
    	}
	}
    fprintf(out, "],\n");
	
    // planting trees...
	bool first = true;
    fprintf(out, "\"objects\":[");
	for (int i = 1; i < data.height - 1; i++) {
		for (int j = 1 + (i == 1); j < data.width - 1; j++) {
			if (rand() % 3000 < data.crate_chance) {
				if (!first) fputc(',', out);
				first = false;
            	fprintf(out, "{\"x\":%d,\"y\":%d,\"w\":1,\"h\":1,\"t\":{\"id\":%d,\"p\":[]},\"c\":null}", j, i,
					(rand() % 20 < data.crate_chance) ? ((rand() % 2 == 0) ? HEAVY_FURNITURE : CRATES) : ((rand() % 2 == 0) ? LOW_FURNITURE : SMALL_FURNITURE));

			}
			else if (rand() % 100 < data.plant_chance) {
				if (!first) fputc(',', out);
				first = false;
            	fprintf(out, "{\"x\":%d,\"y\":%d,\"w\":1,\"h\":1,\"t\":{\"id\":%d,\"p\":[]},\"c\":null}", j, i,
					(rand() % 20 < data.plant_chance) ? TREE : BUSH);
			} else if (rand() % 100 < data.stone_chance) {
				if (!first) fputc(',', out);
				first = false;
            	fprintf(out, "{\"x\":%d,\"y\":%d,\"w\":1,\"h\":1,\"t\":{\"id\":%d,\"p\":[]},\"c\":null}", j, i,
					(rand() % 20 < data.stone_chance) ? HEAVY_ROCKS : SMALL_ROCKS);
			}
        }
	}
	// ...and placing doors
	int door_id = 0;
	for (int pos : door_pos) {
		int offset = door_offset_by_pos(pos, data);
		int i = offset / data.width, j = offset % data.width;
		if (!first) fputc(',', out);
		first = false;
		if (door_id == input_door) {
        	fprintf(out, "{\"x\":%d,\"y\":%d,\"w\":1,\"h\":1,\"t\":{\"id\":%d,\"p\":[]},\"c\":%d}", j, i, DOOR, RGB(0xae, 0x10, 0x10));
		} else {
        	fprintf(out, "{\"x\":%d,\"y\":%d,\"w\":1,\"h\":1,\"t\":{\"id\":%d,\"p\":[]},\"c\":null}", j, i, DOOR);
		}
		door_id++;
	}
	fprintf(out, "],\n");

	// and door labels
	fprintf(out, "\"labels\":[");
	door_id = 0;
	first = true;
	for (int pos : door_pos) {
		if (door_id != input_door) {
			int offset = door_offset_by_pos(pos, data);
			int i = offset / data.width, j = offset % data.width;
			double x = j, y = i;
			if (i == 0) { // top
				x += 0.5; y += 1.5;
			} else if (j == data.width - 1) { // right
				x -= 0.5; y += 0.5;
			} else if (i == data.height - 1) { // bottom
				x += 0.5; y -= 0.5;
			} else { // left
				x += 1.5; y += 0.5;
			}
			if (!first) fputc(',', out);
			first = false;
			if (door_id < input_door) {
				fprintf(out, "{\"x\":%lf,\"y\":%lf,\"s\":\"%d\",\"gm\":false,", x, y, (data.door_num - 1) - (input_door - door_id));
			} else {
				fprintf(out, "{\"x\":%lf,\"y\":%lf,\"s\":\"%d\",\"gm\":false,", x, y, door_id - input_door - 1);
			}
			fprintf(out, "\"font\":{\"id\":2,\"p\":[]},\"fontSize\":{\"id\":1,\"p\":[]},\"fontColor\":%d,", data.color.font);
			fprintf(out, "\"display\":{\"id\":0,\"p\":[]},\"bubbleGX\":-1,\"bubbleGY\":-1,\"outline\":{\"id\":1,\"p\":[]}}");
		}
		door_id++;
	}	
	fprintf(out, "],\n");

	// nothing else for now
	
    fprintf(out, "\"icons\":[],\n");
    fprintf(out, "\"lights\":[],\n");
    fprintf(out, "\"mobs\":[],\n");
    fprintf(out, "\"pools\":[],\n");
    fprintf(out, "\"imageLib\":{\"images\":[]}\n}");
    /*  "labels":[
	    ],
	    "icons":[
		    {
			    "x":25.25,
			    "y":3.438,
			    "icon":{
				    "id":0,
				    "p":[
	    				"star"
    				]
			    },
			    "ang":0,
			    "scale":1,
			    "filter":{
				    "id":1,
				    "p":[
					
				    ]
			    },
			    "depth":{
				    "id":1,
				    "p":[
					
				    ]
			    }
		    }
	    ],
	    "lights":[
		
	    ],
    	"mobs":[
		
	    ],
	    "pools":[
		    "201:0",
		    "203:1",
		    "205:2"
	    ],
	    "imageLib":{
		    "images":[
			
		    ]
	    }
    }*/
}