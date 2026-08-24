#!/usr/bin/perl

# Tests for condition-aware upstream fields used by http extra variables.

###############################################################################

use warnings;
use strict;

use Test::More;

use lib 'lib';
use Test::Nginx qw/ :DEFAULT /;

###############################################################################

select STDERR; $| = 1;
select STDOUT; $| = 1;

my $t = Test::Nginx->new()->has(qw/http proxy cache ngx_condition_module
	ngx_http_extra_variables_module/)->plan(12);

$t->write_file_expand('nginx.conf', <<'EOF');

%%TEST_GLOBALS%%

daemon off;

events {
}

http {
    %%TEST_GLOBALS_HTTP%%

    proxy_cache_path %%TESTDIR%%/cache levels=1:2 keys_zone=NAME:1m;

    server {
        listen       127.0.0.1:8080;
        server_name  localhost;

        condition ignore_headers str_in $http_x_case ignore_headers;
        condition ignore_control str_in $http_x_case ignore_control;

        location /headers/ {
            proxy_pass http://127.0.0.1:8081;
            proxy_cache NAME;
            proxy_cache_valid 200 1h;
            add_header X-TTL $upstream_cache_ttl always;

            when ignore_headers {
                proxy_ignore_headers Cache-Control;
            }
        }

        location /control/ {
            proxy_pass http://127.0.0.1:8081;
            proxy_cache NAME;
            proxy_cache_valid 200 1h;
            add_header X-TTL $upstream_cache_ttl always;

            when ignore_control {
                proxy_ignore_cache_control max-age;
            }
        }

        location /expires/ {
            proxy_pass http://127.0.0.1:8081;
            proxy_cache NAME;
            proxy_cache_valid 200 1h;
            add_header X-TTL $upstream_cache_ttl always;

            when ignore_headers {
                proxy_ignore_headers X-Accel-Expires;
            }
        }
    }

    server {
        listen       127.0.0.1:8081;
        server_name  localhost;

        location /expires/ {
            add_header X-Accel-Expires 600;
            return 200 cached;
        }

        location / {
            add_header Cache-Control "max-age=600";
            return 200 cached;
        }
    }
}

EOF

$t->run();

###############################################################################

my $ttl = ttl('/headers/default');
cmp_ok($ttl, '>=', 590, 'proxy_ignore_headers default lower bound');
cmp_ok($ttl, '<=', 600, 'proxy_ignore_headers default upper bound');

$ttl = ttl('/headers/condition', 'ignore_headers');
cmp_ok($ttl, '>=', 3590, 'proxy_ignore_headers condition lower bound');
cmp_ok($ttl, '<=', 3600, 'proxy_ignore_headers condition upper bound');

$ttl = ttl('/control/default');
cmp_ok($ttl, '>=', 590, 'proxy_ignore_cache_control default lower bound');
cmp_ok($ttl, '<=', 600, 'proxy_ignore_cache_control default upper bound');

$ttl = ttl('/control/condition', 'ignore_control');
cmp_ok($ttl, '>=', 3590,
	'proxy_ignore_cache_control condition lower bound');
cmp_ok($ttl, '<=', 3600,
	'proxy_ignore_cache_control condition upper bound');

$ttl = ttl('/expires/default');
cmp_ok($ttl, '>=', 590, 'X-Accel-Expires default lower bound');
cmp_ok($ttl, '<=', 600, 'X-Accel-Expires default upper bound');

$ttl = ttl('/expires/condition', 'ignore_headers');
cmp_ok($ttl, '>=', 3590, 'X-Accel-Expires condition lower bound');
cmp_ok($ttl, '<=', 3600, 'X-Accel-Expires condition upper bound');

###############################################################################

sub ttl {
	my ($uri, $case) = @_;
	my $header = defined $case ? "X-Case: $case\r\n" : '';
	my $response = get($uri, $header);

	return $1 if $response =~ /^X-TTL: (\d+)\x0d?$/mi;
	die "no X-TTL header in response: $response";
}

sub get {
	my ($uri, $header) = @_;
	$header = '' unless defined $header;

	return http("GET $uri HTTP/1.1\r\nHost: localhost\r\n"
		. $header . "Connection: close\r\n\r\n");
}
