-- DROP SCHEMA "data";

CREATE SCHEMA "data" AUTHORIZATION ml_role;

-- DROP TYPE "data"."000001";

CREATE TYPE "data"."000001" AS (
	idx int8,
	f1 float8,
	f2 float8,
	f3 float8,
	f4 float8,
	f5 float8);

-- DROP TYPE "data"."000002";

CREATE TYPE "data"."000002" AS (
	idx int8,
	f1 float8,
	f2 float8,
	f3 float8,
	f4 float8,
	f5 float8);

-- DROP TYPE "data"."000003";

CREATE TYPE "data"."000003" AS (
	idx int8,
	f1 float8,
	f2 float8,
	f3 float8,
	f4 float8,
	f5 float8);

-- DROP TYPE "data"."000004";

CREATE TYPE "data"."000004" AS (
	idx int8,
	f1 float8,
	f2 float8,
	f3 float8,
	f4 float8,
	f5 float8);

-- DROP TYPE "data".master_index;

CREATE TYPE "data".master_index AS (
	idx int8);

-- DROP TYPE "data".test_linear;

CREATE TYPE "data".test_linear AS (
	idx int8,
	x float8,
	y float8);

-- DROP TYPE "data"."_000001";

CREATE TYPE "data"."_000001" (
	INPUT = array_in,
	OUTPUT = array_out,
	RECEIVE = array_recv,
	SEND = array_send,
	ANALYZE = array_typanalyze,
	ALIGNMENT = 8,
	STORAGE = any,
	CATEGORY = A,
	ELEMENT = "data"."000001",
	DELIMITER = ',');

-- DROP TYPE "data"."_000002";

CREATE TYPE "data"."_000002" (
	INPUT = array_in,
	OUTPUT = array_out,
	RECEIVE = array_recv,
	SEND = array_send,
	ANALYZE = array_typanalyze,
	ALIGNMENT = 8,
	STORAGE = any,
	CATEGORY = A,
	ELEMENT = "data"."000002",
	DELIMITER = ',');

-- DROP TYPE "data"."_000003";

CREATE TYPE "data"."_000003" (
	INPUT = array_in,
	OUTPUT = array_out,
	RECEIVE = array_recv,
	SEND = array_send,
	ANALYZE = array_typanalyze,
	ALIGNMENT = 8,
	STORAGE = any,
	CATEGORY = A,
	ELEMENT = "data"."000003",
	DELIMITER = ',');

-- DROP TYPE "data"."_000004";

CREATE TYPE "data"."_000004" (
	INPUT = array_in,
	OUTPUT = array_out,
	RECEIVE = array_recv,
	SEND = array_send,
	ANALYZE = array_typanalyze,
	ALIGNMENT = 8,
	STORAGE = any,
	CATEGORY = A,
	ELEMENT = "data"."000004",
	DELIMITER = ',');

-- DROP TYPE "data"."_master_index";

CREATE TYPE "data"."_master_index" (
	INPUT = array_in,
	OUTPUT = array_out,
	RECEIVE = array_recv,
	SEND = array_send,
	ANALYZE = array_typanalyze,
	ALIGNMENT = 8,
	STORAGE = any,
	CATEGORY = A,
	ELEMENT = "data".master_index,
	DELIMITER = ',');

-- DROP TYPE "data"."_test_linear";

CREATE TYPE "data"."_test_linear" (
	INPUT = array_in,
	OUTPUT = array_out,
	RECEIVE = array_recv,
	SEND = array_send,
	ANALYZE = array_typanalyze,
	ALIGNMENT = 8,
	STORAGE = any,
	CATEGORY = A,
	ELEMENT = "data".test_linear,
	DELIMITER = ',');
-- "data"."000001" definition

-- Drop table

-- DROP TABLE "data"."000001";

CREATE TABLE "data"."000001" (
	idx int8 NOT NULL,
	f1 float8 NULL,
	f2 float8 NULL,
	f3 float8 NULL,
	f4 float8 NULL,
	f5 float8 NULL,
	CONSTRAINT "000001_idx_key" UNIQUE (idx)
);


-- "data"."000002" definition

-- Drop table

-- DROP TABLE "data"."000002";

CREATE TABLE "data"."000002" (
	idx int8 NOT NULL,
	f1 float8 NULL,
	f2 float8 NULL,
	f3 float8 NULL,
	f4 float8 NULL,
	f5 float8 NULL,
	CONSTRAINT "000002_idx_key" UNIQUE (idx)
);


-- "data"."000003" definition

-- Drop table

-- DROP TABLE "data"."000003";

CREATE TABLE "data"."000003" (
	idx int8 NOT NULL,
	f1 float8 NULL,
	f2 float8 NULL,
	f3 float8 NULL,
	f4 float8 NULL,
	f5 float8 NULL,
	CONSTRAINT "000003_un" UNIQUE (idx)
);
CREATE UNIQUE INDEX "000003_idx_IDX" ON data."000003" USING btree (idx);


-- "data"."000004" definition

-- Drop table

-- DROP TABLE "data"."000004";

CREATE TABLE "data"."000004" (
	idx int8 NOT NULL,
	f1 float8 NULL,
	f2 float8 NULL,
	f3 float8 NULL,
	f4 float8 NULL,
	f5 float8 NULL,
	CONSTRAINT "000004_idx_key" UNIQUE (idx)
);


-- "data".master_index definition

-- Drop table

-- DROP TABLE "data".master_index;

CREATE TABLE "data".master_index (
	idx int8 NOT NULL,
	CONSTRAINT master_index_pk PRIMARY KEY (idx)
);


-- "data".test_linear definition

-- Drop table

-- DROP TABLE "data".test_linear;

CREATE TABLE "data".test_linear (
	idx int8 NOT NULL,
	x float8 NULL,
	y float8 NULL,
	CONSTRAINT test_xor_idx_key UNIQUE (idx)
);