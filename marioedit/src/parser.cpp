#include <marioedit/parser.hpp>

#include <MM2/level.h>
#include <SMM2CourseDecryptor/decrypt.h>
#include <gzip/decompress.hpp>

#include <algorithm>
#include <codecvt>
#include <filesystem>
#include <fstream>

namespace MarioEdit {
	namespace Level {
		std::string DecryptLevel(std::string path) {
			constexpr bool log = true;

			uintmax_t filesize = std::filesystem::file_size(path);
			std::string content;

			if(log) {
				fmt::print("Level filesize is {}\n", filesize);
			}

			if(filesize == 0x5C000) {
				if(log) {
					puts("File is encrypted");
				}

				std::ifstream ifs(path, std::ios::binary);
				std::string input(
					(std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
				Parser::DecryptLevelData(input, content);
			} else {
				// First, check if compressed
				FILE* magicFile = fopen(path.c_str(), "rb");
				uint8_t zlibMagic[2];
				uint8_t validZlibMagic1[2] = { 0x78, 0x01 };
				uint8_t validZlibMagic2[2] = { 0x78, 0x5E };
				uint8_t validZlibMagic3[2] = { 0x78, 0x9C };
				uint8_t validZlibMagic4[2] = { 0x78, 0xDA };
				fread(zlibMagic, 2, 1, magicFile);
				fclose(magicFile);
				if(memcmp(zlibMagic, validZlibMagic1, 2) == 0
					|| memcmp(zlibMagic, validZlibMagic2, 2) == 0
					|| memcmp(zlibMagic, validZlibMagic3, 2) == 0
					|| memcmp(zlibMagic, validZlibMagic4, 2) == 0) {
					if(log) {
						puts("File is compressed");
					}

					// Is compressed, decompress and write to new file
					std::ifstream readFile(path, std::ios::in | std::ios::binary);
					std::vector<uint8_t> data((std::istreambuf_iterator<char>(readFile)),
						std::istreambuf_iterator<char>());
					readFile.close();

					content = gzip::decompress((const char*)data.data(), data.size());
				} else {
					puts("File is uncompressed or an unknown format");
				}
			}

			return content;
		}

		Parser::Parser() {
			TileLoc.resize(152);
			for(int i = 0; i < 152; i++) {
				TileLoc[i].resize(4);
			}

			PipeLoc.resize(5);
			for(int i = 0; i < 5; i++) {
				PipeLoc[i].resize(9);
			}

			ObjLocData.resize(2);
			ObjLocData[0].resize(301);
			ObjLocData[1].resize(301);
			for(int i = 0; i < 301; i++) {
				ObjLocData[0][i].resize(301);
				ObjLocData[1][i].resize(301);
			}

			GroundNode.resize(301);
			for(int i = 0; i < 301; i++) {
				GroundNode[i].resize(301);
			}

			TrackYPt.resize(17);
			for(int i = 0; i < 17; i++) {
				TrackYPt[i].resize(4);
			}
		}

		bool Parser::DecryptLevelData(std::string& input, std::string& output) {
			return LevelDecryptor::decrypt(input, output);
		}

		void Parser::LoadLevelData(const std::string& levelData, bool overworld) {
			isOverworld = overworld;
			long long i = 0;
			long long j = 0;

			kaitai::kstream ks(levelData);
			MM2::level_t level(&ks);

			LH.StartY        = level.start_y();
			LH.GoalY         = level.goal_y();
			LH.GoalX         = level.goal_x();
			LH.Timer         = level.timer();
			LH.ClearCA       = level.clear_condition_magnitude();
			LH.DateYY        = level.year();
			LH.DateMM        = level.month();
			LH.DateDD        = level.day();
			LH.DateH         = level.hour();
			LH.DateM         = level.minute();
			LH.AutoscrollSpd = level.autoscroll_speed();
			LH.ClearCC       = level.clear_condition_category();
			LH.ClearCRC      = level.clear_condition();
			LH.GameVer       = level.unk_gamever();
			LH.MFlag         = level.unk_management_flags();
			LH.ClearAttempts = level.clear_attempts();
			LH.ClearTime     = level.clear_time();
			LH.CreationID    = level.unk_creation_id();
			LH.UploadID      = level.unk_upload_id();
			LH.ClearVer      = level.game_version();
			LH.GameStyle     = level.gamestyle();

			std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> stringConverter;
			LH.Name = stringConverter.to_bytes(std::u16string((char16_t*)level.name().data()));
			LH.Desc
				= stringConverter.to_bytes(std::u16string((char16_t*)level.description().data()));

			MM2::level_t::map_t& map = *(overworld ? level.overworld() : level.subworld());

			MapHdr.Theme          = map.theme();
			MapHdr.AutoscrollType = map.autoscroll_type();
			MapHdr.BorFlag        = map.boundary_type();
			MapHdr.Ori            = map.orientation();
			MapHdr.LiqEHeight     = map.liquid_end_height();
			MapHdr.LiqMode        = map.liquid_mode();
			MapHdr.LiqSpd         = map.liquid_speed();
			MapHdr.LiqSHeight     = map.liquid_start_height();
			MapHdr.BorR           = map.boundary_right();
			MapHdr.BorT           = map.boundary_top();
			MapHdr.BorL           = map.boundary_left();
			MapHdr.BorB           = map.boundary_bottom();
			MapHdr.Flag           = map.unk_flag();
			MapHdr.ObjCount       = map.object_count();
			MapHdr.SndCount       = map.sound_effect_count();
			MapHdr.SnakeCount     = map.snake_block_count();
			MapHdr.ClearPipCount  = map.clear_pipe_count();
			MapHdr.CreeperCount   = map.piranha_creeper_count();
			MapHdr.iBlkCount      = map.exclamation_mark_block_count();
			MapHdr.TrackBlkCount  = map.track_block_count();
			MapHdr.GroundCount    = map.ground_count();
			MapHdr.TrackCount     = map.track_count();
			MapHdr.IceCount       = map.ice_count();

			int max_x = 0;
			int max_y = 0;
			MapObj.clear();
			for(int i = 0; i < map.object_count(); i++) {
				auto& obj_ref = *map.objects()->at(i);
				Parser::MapObject newObject;
				newObject.X        = obj_ref.x();
				newObject.Y        = obj_ref.y();
				newObject.W        = obj_ref.width();
				newObject.H        = obj_ref.height();
				newObject.Flag     = obj_ref.flag();
				newObject.CFlag    = obj_ref.cflag();
				newObject.Ex       = obj_ref.ex();
				newObject.ID       = obj_ref.id();
				newObject.CID      = obj_ref.cid();
				newObject.LID      = obj_ref.lid();
				newObject.SID      = obj_ref.sid();
				newObject.LinkType = 0;

				if(newObject.X > max_x) {
					max_x = newObject.X;
				}

				if(newObject.Y > max_y) {
					max_y = newObject.Y;
				}

				MapObj.push_back(newObject);
			}

			// Render objects in the order they are rendered in game, makes semisolids render
			// correctly
			std::sort(std::begin(MapObj), std::end(MapObj), [&](MapObject& a, MapObject& b) {
				return (a.X + (max_y - a.Y - a.H * 160) * max_x)
					   < (b.X + (max_y - b.Y - b.H * 160) * max_x);
			});

			MapSnk.clear();
			for(int i = 0; i < map.snake_block_count(); i++) {
				auto& snake_ref = *map.snakes()->at(i);
				Parser::MapSnakeBlock newSnake;
				newSnake.index     = snake_ref.index();
				newSnake.NodeCount = snake_ref.node_count();

				for(int j = 0; j < newSnake.NodeCount; j++) {
					auto& snake_node_ref = *snake_ref.nodes()->at(j);
					Parser::MapSnakeBlockNode newSnakeNode;
					newSnakeNode.index = snake_node_ref.index();
					newSnakeNode.Dir   = snake_node_ref.direction();

					newSnake.Node.push_back(newSnakeNode);
				}

				MapSnk.push_back(newSnake);
			}

			MapCPipe.clear();
			for(int i = 0; i < map.clear_pipe_count(); i++) {
				auto& clear_pipe_ref = *map.clear_pipes()->at(i);
				Parser::MapClearPipe newClearPipe;
				newClearPipe.Index     = clear_pipe_ref.index();
				newClearPipe.NodeCount = clear_pipe_ref.node_count();

				for(int j = 0; j < newClearPipe.NodeCount; j++) {
					auto& clear_pipe_node_ref = *clear_pipe_ref.nodes()->at(j);
					Parser::MapClearPipeNode newClearPipeNode;
					newClearPipeNode.type  = clear_pipe_node_ref.type();
					newClearPipeNode.index = clear_pipe_node_ref.index();
					newClearPipeNode.X     = clear_pipe_node_ref.x();
					newClearPipeNode.Y     = clear_pipe_node_ref.y();
					newClearPipeNode.W     = clear_pipe_node_ref.width();
					newClearPipeNode.H     = clear_pipe_node_ref.height();
					newClearPipeNode.Dir   = clear_pipe_node_ref.direction();

					newClearPipe.Node.push_back(newClearPipeNode);
				}

				MapCPipe.push_back(newClearPipe);
			}

			MapCrp.clear();
			for(int i = 0; i < map.piranha_creeper_count(); i++) {
				auto& creeper_ref = *map.piranha_creepers()->at(i);
				Parser::MapCreeper newCreeper;
				newCreeper.index     = creeper_ref.index();
				newCreeper.NodeCount = creeper_ref.node_count();

				for(int j = 0; j < newCreeper.NodeCount; j++) {
					auto& creeper_node_ref = *creeper_ref.nodes()->at(j);
					newCreeper.Node.push_back(creeper_node_ref.direction());
				}

				MapCrp.push_back(newCreeper);
			}

			MapMoveBlk.clear();
			for(int i = 0; i < map.exclamation_mark_block_count(); i++) {
				auto& exclamation_block_ref = *map.exclamation_blocks()->at(i);
				Parser::MapMoveBlock newExclamationBlock;
				newExclamationBlock.index     = exclamation_block_ref.index();
				newExclamationBlock.NodeCount = exclamation_block_ref.node_count();

				for(int j = 0; j < newExclamationBlock.NodeCount; j++) {
					auto& exclamation_block_node_ref = *exclamation_block_ref.nodes()->at(j);
					Parser::MapMoveBlockNode newExclamationBlockNode;
					newExclamationBlockNode.p0 = exclamation_block_node_ref.unk1();
					newExclamationBlockNode.p1 = exclamation_block_node_ref.direction();
					newExclamationBlockNode.p2 = exclamation_block_node_ref.unk2();

					newExclamationBlock.Node.push_back(newExclamationBlockNode);
				}

				MapMoveBlk.push_back(newExclamationBlock);
			}

			MapTrackBlk.clear();
			for(int i = 0; i < map.track_block_count(); i++) {
				auto& track_block_ref = *map.track_blocks()->at(i);
				Parser::MapMoveBlock newTrackBlock;
				newTrackBlock.index     = track_block_ref.index();
				newTrackBlock.NodeCount = track_block_ref.node_count();

				for(int j = 0; j < newTrackBlock.NodeCount; j++) {
					auto& track_block_node_ref = *track_block_ref.nodes()->at(j);
					Parser::MapMoveBlockNode newTrackBlockNode;
					newTrackBlockNode.p0 = track_block_node_ref.unk1();
					newTrackBlockNode.p1 = track_block_node_ref.direction();
					newTrackBlockNode.p2 = track_block_node_ref.unk2();

					newTrackBlock.Node.push_back(newTrackBlockNode);
				}

				MapTrackBlk.push_back(newTrackBlock);
			}

			for(int M = 0; M <= 300; M++) {
				for(j = 0; j <= 300; j++) {
					GroundNode[M][j] = 0;
				}
			}

			MapGrd.clear();
			for(int i = 0; i < map.ground_count(); i++) {
				auto& ground_ref = *map.ground()->at(i);
				Parser::MapGround newGround;
				newGround.X   = ground_ref.x();
				newGround.Y   = ground_ref.y();
				newGround.ID  = ground_ref.id();
				newGround.BID = ground_ref.background_id();

				GroundNode[newGround.X + 1][newGround.Y + 1] = 1;

				MapGrd.push_back(newGround);
			}

			if(overworld) {
				for(j = std::round((LH.GoalX - 5) / 10.0); j <= (LH.GoalX - 5) / 10.0 + 9; j++) {
					for(i = 0; i < LH.GoalY; i++) {
						GroundNode[j + 1][i + 1] = 1;
					}
				}

				for(j = 0; j <= 6; j++) {
					for(i = 0; i < LH.StartY; i++) {
						GroundNode[j + 1][i + 1] = 1;
					}
				}
			}

			MapTrk.clear();
			TrackNode.resize(MapHdr.BorR + 3);
			for(int i = 0; i < MapHdr.BorR + 3; i++) {
				TrackNode[i].resize(MapHdr.BorT + 3);
			}

			for(int i = 0; i < map.track_count(); i++) {
				auto& track_ref = *map.tracks()->at(i);
				Parser::MapTrack newTrack;
				newTrack.UN   = track_ref.unk1();
				newTrack.Flag = track_ref.flags();

				int TX = track_ref.x();
				if(TX == 255) {
					newTrack.X = 0;
				} else {
					newTrack.X = (unsigned char)(TX + 1);
				}

				int TY = track_ref.y();
				if(TY == 255) {
					newTrack.Y = 0;
				} else {
					newTrack.Y = (unsigned char)(TY + 1);
				}

				newTrack.Type = track_ref.type();
				newTrack.LID  = track_ref.lid();
				newTrack.K0   = track_ref.unk2();
				newTrack.K1   = track_ref.unk3();

				switch(newTrack.Type) {
				case 0:
					TrackNode[newTrack.X][newTrack.Y + 1] += 1;
					TrackNode[newTrack.X + 2][newTrack.Y + 1] += 1;
					newTrack.F0 = (unsigned char)(((newTrack.K0 + 0x10000) / 0x80) % 2);
					newTrack.F1 = (unsigned char)(((newTrack.K1 + 0x10000) / 0x80) % 2);
					break;
				case 1:
					TrackNode[newTrack.X + 1][newTrack.Y + 2] += 1;
					TrackNode[newTrack.X + 1][newTrack.Y] += 1;
					newTrack.F0 = (unsigned char)(((newTrack.K0 + 0x10000) / 0x80) % 2);
					newTrack.F1 = (unsigned char)(((newTrack.K1 + 0x10000) / 0x80) % 2);
					break;
				case 2:
				case 4:
				case 5:
					TrackNode[newTrack.X][newTrack.Y + 2] += 1;
					TrackNode[newTrack.X + 2][newTrack.Y] += 1;
					newTrack.F0 = (unsigned char)(((newTrack.K0 + 0x10000) / 0x80) % 2);
					newTrack.F1 = (unsigned char)(((newTrack.K1 + 0x10000) / 0x80) % 2);
					break;
				case 3:
				case 6:
				case 7:
					TrackNode[newTrack.X + 2][newTrack.Y + 2] += 1;
					TrackNode[newTrack.X][newTrack.Y] += 1;
					newTrack.F0 = (unsigned char)(((newTrack.K0 + 0x10000) / 0x80) % 2);
					newTrack.F1 = (unsigned char)(((newTrack.K1 + 0x10000) / 0x80) % 2);
					break;
				case 8:
					TrackNode[newTrack.X][newTrack.Y + 2] += 1;
					TrackNode[newTrack.X + 4][newTrack.Y] += 1;
					TrackNode[newTrack.X + 4][newTrack.Y + 4] += 1;
					newTrack.F0 = (unsigned char)(((newTrack.K1 + 0x10000) / 0x80) % 2);
					newTrack.F1 = (unsigned char)(((newTrack.K0 + 0x10000) / 0x80) % 2);
					newTrack.F2 = (unsigned char)(((newTrack.K0 + 0x10000) / 0x8000) % 2);
					break;
				case 9:
					TrackNode[newTrack.X][newTrack.Y] += 1;
					TrackNode[newTrack.X][newTrack.Y + 4] += 1;
					TrackNode[newTrack.X + 4][newTrack.Y + 2] += 1;
					newTrack.F0 = (unsigned char)(((newTrack.K1 + 0x10000) / 0x40) % 2);
					newTrack.F1 = (unsigned char)(((newTrack.K1 + 0x10000) / 0x2) % 2);
					newTrack.F2 = (unsigned char)(((newTrack.K0 + 0x10000) / 0x80) % 2);
					break;
				case 10:
					TrackNode[newTrack.X][newTrack.Y] += 1;
					TrackNode[newTrack.X + 2][newTrack.Y + 4] += 1;
					TrackNode[newTrack.X + 4][newTrack.Y] += 1;
					newTrack.F0 = (unsigned char)(1 - ((newTrack.K0 + 0x10000) / 0x4000) % 2);
					newTrack.F1 = (unsigned char)(((newTrack.K1 + 0x10000) / 0x40) % 2);
					newTrack.F2 = (unsigned char)(((newTrack.K0 + 0x10000) / 0x80) % 2);
					break;
				case 11:
					TrackNode[newTrack.X + 2][newTrack.Y] += 1;
					TrackNode[newTrack.X][newTrack.Y + 4] += 1;
					TrackNode[newTrack.X + 4][newTrack.Y + 4] += 1;
					newTrack.F0 = (unsigned char)(((newTrack.K0 + 0x10000) / 0x80) % 2);
					newTrack.F1 = (unsigned char)(((newTrack.K1 + 0x10000) / 0x2) % 2);
					newTrack.F2 = (unsigned char)(((newTrack.K1 + 0x10000) / 0x40) % 2);
					break;
				case 12:
					TrackNode[newTrack.X][newTrack.Y + 2] += 1;
					TrackNode[newTrack.X + 4][newTrack.Y] += 1;
					TrackNode[newTrack.X + 4][newTrack.Y + 4] += 1;
					newTrack.F0 = (unsigned char)(((newTrack.K1 + 0x10000) / 0x800) % 2);
					newTrack.F1 = (unsigned char)(((newTrack.K0 + 0x10000) / 0x80) % 2);
					newTrack.F2 = (unsigned char)(((newTrack.K0 + 0x10000) / 0x1000) % 2);
					break;
				case 13:
					TrackNode[newTrack.X][newTrack.Y] += 1;
					TrackNode[newTrack.X][newTrack.Y + 4] += 1;
					TrackNode[newTrack.X + 4][newTrack.Y + 2] += 1;
					newTrack.F0 = (unsigned char)(((newTrack.K1 + 0x10000) / 0x800) % 2);
					newTrack.F1 = (unsigned char)(((newTrack.K0 + 0x10000) / 0x1000) % 2);
					newTrack.F2 = (unsigned char)(((newTrack.K0 + 0x10000) / 0x80) % 2);
					break;
				case 14:
					TrackNode[newTrack.X][newTrack.Y] += 1;
					TrackNode[newTrack.X + 4][newTrack.Y] += 1;
					TrackNode[newTrack.X + 2][newTrack.Y + 4] += 1;
					newTrack.F0 = (unsigned char)(((newTrack.K0 + 0x10000) / 0x1000) % 2);
					newTrack.F1 = (unsigned char)(((newTrack.K1 + 0x10000) / 0x800) % 2);
					newTrack.F2 = (unsigned char)(((newTrack.K0 + 0x10000) / 0x80) % 2);
					break;
				case 15:
					TrackNode[newTrack.X + 2][newTrack.Y] += 1;
					TrackNode[newTrack.X][newTrack.Y + 4] += 1;
					TrackNode[newTrack.X + 4][newTrack.Y + 4] += 1;
					newTrack.F0 = (unsigned char)(((newTrack.K0 + 0x10000) / 0x80) % 2);
					newTrack.F1 = (unsigned char)(((newTrack.K0 + 0x10000) / 0x1000) % 2);
					newTrack.F2 = (unsigned char)(((newTrack.K1 + 0x10000) / 0x800) % 2);
					break;
				}

				MapTrk.push_back(newTrack);
			}

			MapIce.clear();
			for(int i = 0; i < map.ice_count(); i++) {
				auto& ice_ref = *map.icicles()->at(i);
				Parser::MapGround newIce;
				newIce.X  = ice_ref.x();
				newIce.Y  = ice_ref.y();
				newIce.ID = ice_ref.type();

				MapIce.push_back(newIce);
			}

			fmt::print("Done parsing {}\n", LH.Name);
		}

		const char* ObjEng[] = { "Goomba", "Koopa", "Piranha Flower", "Hammer Bro", "Block",
			"? Block", "Stone", "Hard Block", "Coin", "Pipe", "Spring", "Lift", "Thwomp",
			"Bullet Bill Blaster", "Mushroom Platform", "Bob-omb", "Semisolid Platform", "Bridge",
			"P Switch", "POW", "Super Mushroom", "Donut Block", "Cloud", "Note Block", "Fire Bar",
			"Spiny", "Hard Block", "Goal", "Buzzy Beetle", "Hidden Block", "Lakitu", "Lakitu Cloud",
			"Banzai Bill", "1UP", "Fire Flower", "Super Star", "Lava Lift", "Starting Brick",
			"Starting Arrow", "Magikoopa", "Spike Top", "Boo", "Clown Car", "Spikes",
			"Big Mushroom", "Shoe Goomba", "Dry Bones", "Cannon", "Blooper", "Castle Bridge",
			"Jumping Machine", "Skipsqueak", "Wiggler", "Conveyor Belt", "Burner", "Door",
			"Cheep Cheep", "Muncher", "Rocky Wrench", "Track", "Lava Bubble", "Chain Chomp",
			"Bowser", "Ice Block", "Vine", "Stingby", "Arrow", "One-Way", "Saw", "Player",
			"Big Coin", "Half Collision Platform", "Koopa Car", "Cinobio (not yet named)",
			"Spike/Ball", "Stone", "Twister", "Boom Boom", "Pokey", "P Block", "Sprint Platform",
			"SMB2 Mushroom", "Donut", "Skewer", "Snake Block", "Spike Block", "Charvaargh",
			"Slight Slope", "Steep Slope", "Reel Camera", "Checkpoint Flag", "Seesaw", "Red Coin",
			"Clear Pipe", "Conveyor Belt", "Key", "Ant Trooper", "Warp Box", "Bowser Jr",
			"ON/OFF Block", "Dotted-Line Block", "Water Marker", "Monty Mole", "Fish Bone",
			"Angry Sun", "Swinging Claw", "Tree", "Piranha Creeper", "Blinking Block",
			"Sound Effect", "Spike Block", "Mechakoopa", "Crate", "Mushroom Trampoline",
			"Porkupuffer", "Cinobic", "Super Hammer", "Bully", "Icicle", "! Block", "Lemmy",
			"Morton", "Larry", "Wendy", "Iggy", "Roy", "Ludwig", "Cannon Box", "Propeller Box",
			"Goomba Mask", "Bullet Bill Mask", "Red POW Box", "ON/OFF Trampoline" };

		ParserMappings* levelMappings = new ParserMappings();
	}
}